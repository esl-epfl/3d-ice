/* A balanced bounding-box hierarchy: one reference per cell, no bin replication.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "nonuniform_spatial_index.h"

#define SPATIAL_LEAF_SIZE 8u

typedef struct SpatialNode_t
{
    ChipDimension_t Box[4];
    uint32_t First; /* First cell in a leaf, left child in an internal node. */
    uint32_t Count; /* Zero for internal nodes. */
    uint32_t Right;
} SpatialNode_t;

struct NonuniformSpatialIndex_t
{
    CellIndex_t Start, End;
    CellIndex_t *Ids;
    SpatialNode_t *Nodes;
    uint32_t NodeCount;
    const ChipDimension_t (*Positions)[4];
};

static double center
(
    const NonuniformSpatialIndex_t *index, CellIndex_t id, unsigned axis
)
{
    /* Avoid overflow when both endpoints are large and have the same sign. */
    return 0.5 * index->Positions[id][axis]
         + 0.5 * index->Positions[id][axis + 2u];
}

static bool precedes
(
    const NonuniformSpatialIndex_t *index,
    CellIndex_t a, CellIndex_t b, unsigned axis
)
{
    double ca = center (index, a, axis), cb = center (index, b, axis);
    return ca < cb || (ca == cb && a < b);
}

static void swap_ids (CellIndex_t *a, CellIndex_t *b)
{
    CellIndex_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static void sift_down
(
    NonuniformSpatialIndex_t *index,
    size_t begin, size_t root, size_t count, unsigned axis
)
{
    while (root < count / 2u)
    {
        size_t child = root * 2u + 1u;
        if (child + 1u < count
            && precedes (index, index->Ids[begin + child],
                         index->Ids[begin + child + 1u], axis))
            child++;
        if (!precedes (index, index->Ids[begin + root],
                       index->Ids[begin + child], axis))
            return;
        swap_ids (&index->Ids[begin + root], &index->Ids[begin + child]);
        root = child;
    }
}

/* Deterministic median selection. The heap-sort fallback bounds work for
 * adversarial input orders without extra allocations or global comparators. */
static void select_median
(
    NonuniformSpatialIndex_t *index,
    size_t begin, size_t end, size_t median, unsigned axis
)
{
    unsigned budget = 0u;
    for (size_t n = end - begin; n > 1u; n >>= 1u)
        budget += 2u;
    while (end - begin > 1u)
    {
        if (budget-- == 0u)
        {
            size_t count = end - begin;
            for (size_t root = count / 2u; root > 0u; root--)
                sift_down (index, begin, root - 1u, count, axis);
            for (size_t n = count; n > 1u; n--)
            {
                swap_ids (&index->Ids[begin], &index->Ids[begin + n - 1u]);
                sift_down (index, begin, 0u, n - 1u, axis);
            }
            return;
        }
        size_t mid = begin + (end - begin) / 2u;
        CellIndex_t a = index->Ids[begin], b = index->Ids[mid];
        CellIndex_t c = index->Ids[end - 1u];
        if (precedes (index, b, a, axis)) swap_ids (&a, &b);
        if (precedes (index, c, b, axis)) swap_ids (&b, &c);
        if (precedes (index, b, a, axis)) swap_ids (&a, &b);
        CellIndex_t pivot = b;
        size_t left = begin, right = end;
        while (left < right)
        {
            while (left < right
                   && precedes (index, index->Ids[left], pivot, axis))
                left++;
            while (left < right
                   && precedes (index, pivot, index->Ids[right - 1u], axis))
                right--;
            if (left < right)
            {
                swap_ids (&index->Ids[left], &index->Ids[right - 1u]);
                left++;
                right--;
            }
        }
        if (median < right)
            end = right;
        else if (median >= left)
            begin = left;
        else
            return;
    }
}

static uint32_t build_node
(
    NonuniformSpatialIndex_t *index, uint32_t begin, uint32_t end
)
{
    uint32_t node_id = index->NodeCount++;
    SpatialNode_t *node = &index->Nodes[node_id];
    memcpy (node->Box, index->Positions[index->Ids[begin]], sizeof(node->Box));
    double low[2], high[2];
    for (unsigned axis = 0; axis < 2u; axis++)
        low[axis] = high[axis] = center (index, index->Ids[begin], axis);
    for (uint32_t i = begin + 1u; i < end; i++)
    {
        const ChipDimension_t *box = index->Positions[index->Ids[i]];
        for (unsigned axis = 0; axis < 2u; axis++)
        {
            if (box[axis] < node->Box[axis]) node->Box[axis] = box[axis];
            if (box[axis + 2u] > node->Box[axis + 2u])
                node->Box[axis + 2u] = box[axis + 2u];
            double value = center (index, index->Ids[i], axis);
            if (value < low[axis]) low[axis] = value;
            if (value > high[axis]) high[axis] = value;
        }
    }
    node->Count = end - begin;
    node->First = begin;
    node->Right = 0u;
    if (node->Count > SPATIAL_LEAF_SIZE)
    {
        unsigned axis = high[1] - low[1] > high[0] - low[0] ? 1u : 0u;
        uint32_t mid = begin + (end - begin) / 2u;
        select_median (index, begin, end, mid, axis);
        node->Count = 0u;
        node->First = build_node (index, begin, mid);
        node->Right = build_node (index, mid, end);
    }
    return node_id;
}

void nonuniform_spatial_cache_init
(
    NonuniformSpatialCache_t *cache,
    const ChipDimension_t (*positions)[4]
)
{
    memset (cache, 0, sizeof(*cache));
    cache->Positions = positions;
}

void nonuniform_spatial_cache_destroy (NonuniformSpatialCache_t *cache)
{
    for (size_t i = 0; i < cache->Size; i++)
    {
        free (cache->Indexes[i].Ids);
        free (cache->Indexes[i].Nodes);
    }
    free (cache->Indexes);
    memset (cache, 0, sizeof(*cache));
}

const NonuniformSpatialIndex_t *nonuniform_spatial_cache_get
(
    NonuniformSpatialCache_t *cache, CellIndex_t start, CellIndex_t end
)
{
    if (end < start || (end != start && cache->Positions == NULL))
        return NULL;
    for (size_t i = 0; i < cache->Size; i++)
        if (cache->Indexes[i].Start == start && cache->Indexes[i].End == end)
            return &cache->Indexes[i];
    if (cache->Size == cache->Capacity)
    {
        size_t capacity = cache->Capacity == 0u ? 16u : cache->Capacity * 2u;
        if (capacity < cache->Capacity
            || capacity > SIZE_MAX / sizeof(*cache->Indexes))
            return NULL;
        NonuniformSpatialIndex_t *indexes = realloc
            (cache->Indexes, capacity * sizeof(*indexes));
        if (indexes == NULL) return NULL;
        cache->Indexes = indexes;
        cache->Capacity = capacity;
    }
    NonuniformSpatialIndex_t index = {0};
    index.Start = start;
    index.End = end;
    index.Positions = cache->Positions;
    size_t count = (size_t) end - start;
    if (count != 0u)
    {
        /* Balanced splitting to <=8 cells produces at most this many nodes. */
        size_t leaves = 1u;
        size_t needed = count / SPATIAL_LEAF_SIZE
                      + (count % SPATIAL_LEAF_SIZE != 0u);
        while (leaves < needed)
        {
            if (leaves > SIZE_MAX / 2u) return NULL;
            leaves *= 2u;
        }
        if (leaves > SIZE_MAX / 2u) return NULL;
        size_t nodes = 2u * leaves - 1u;
        if (count > SIZE_MAX / sizeof(*index.Ids)
            || nodes > SIZE_MAX / sizeof(*index.Nodes)
            || nodes > UINT32_MAX)
            return NULL;
        index.Ids = malloc (count * sizeof(*index.Ids));
        index.Nodes = malloc (nodes * sizeof(*index.Nodes));
        if (index.Ids == NULL || index.Nodes == NULL)
        {
            free (index.Ids);
            free (index.Nodes);
            return NULL;
        }
        for (size_t i = 0; i < count; i++)
        {
            CellIndex_t id = start + (CellIndex_t) i;
            const ChipDimension_t *box = cache->Positions[id];
            if (!isfinite(box[0]) || !isfinite(box[1])
                || !isfinite(box[2]) || !isfinite(box[3])
                || box[0] > box[2] || box[1] > box[3])
            {
                free (index.Ids);
                free (index.Nodes);
                return NULL;
            }
            index.Ids[i] = id;
        }
        build_node (&index, 0u, (uint32_t) count);
    }
    cache->Indexes[cache->Size] = index;
    return &cache->Indexes[cache->Size++];
}

static bool intersects (const double a[4], const double b[4])
{
    return a[0] <= b[2] && b[0] <= a[2]
        && a[1] <= b[3] && b[1] <= a[3];
}

static bool append_candidate
(
    NonuniformSpatialCandidates_t *candidates, CellIndex_t id
)
{
    if (candidates->Size == candidates->Capacity)
    {
        size_t capacity = candidates->Capacity == 0u
            ? 32u : candidates->Capacity * 2u;
        if (capacity < candidates->Capacity
            || capacity > SIZE_MAX / sizeof(*candidates->Values))
            return false;
        CellIndex_t *values = realloc
            (candidates->Values, capacity * sizeof(*values));
        if (values == NULL) return false;
        candidates->Values = values;
        candidates->Capacity = capacity;
    }
    candidates->Values[candidates->Size++] = id;
    return true;
}

static bool query_node
(
    const NonuniformSpatialIndex_t *index, uint32_t node_id,
    const double box[4], CellIndex_t minimum_id,
    NonuniformSpatialCandidates_t *candidates,
    NonuniformSpatialQueryStats_t *stats
)
{
    const SpatialNode_t *node = &index->Nodes[node_id];
    if (stats != NULL) stats->NodesVisited++;
    if (!intersects (node->Box, box)) return true;
    if (node->Count == 0u)
        return query_node (index, node->First, box, minimum_id, candidates, stats)
            && query_node (index, node->Right, box, minimum_id, candidates, stats);
    for (uint32_t i = node->First; i < node->First + node->Count; i++)
    {
        CellIndex_t id = index->Ids[i];
        if (id < minimum_id) continue;
        if (stats != NULL) stats->CellsTested++;
        if (intersects (index->Positions[id], box)
            && !append_candidate (candidates, id))
            return false;
    }
    return true;
}

static int compare_ids (const void *a, const void *b)
{
    CellIndex_t x = *(const CellIndex_t *) a, y = *(const CellIndex_t *) b;
    return (x > y) - (x < y);
}

bool nonuniform_spatial_query
(
    const NonuniformSpatialIndex_t *index,
    const ChipDimension_t box[4], ChipDimension_t padding,
    CellIndex_t minimum_id, NonuniformSpatialCandidates_t *candidates,
    NonuniformSpatialQueryStats_t *stats
)
{
    candidates->Size = 0u;
    if (index == NULL || !isfinite(padding) || padding < 0.0)
        return false;
    if (index->NodeCount == 0u) return true;
    double query[4];
    for (unsigned axis = 0; axis < 2u; axis++)
    {
        /* Outward rounding also protects exact face contact at large origins. */
        query[axis] = nextafter (box[axis] - padding, -INFINITY);
        query[axis + 2u] = nextafter (box[axis + 2u] + padding, INFINITY);
    }
    if (!query_node (index, 0u, query, minimum_id, candidates, stats))
    {
        candidates->Size = 0u;
        return false;
    }
    if (candidates->Size > 1u)
        qsort (candidates->Values, candidates->Size,
               sizeof(*candidates->Values), compare_ids);
    return true;
}

void nonuniform_spatial_candidates_destroy
(
    NonuniformSpatialCandidates_t *candidates
)
{
    free (candidates->Values);
    memset (candidates, 0, sizeof(*candidates));
}
