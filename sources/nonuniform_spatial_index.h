/* Private, temporary geometry index for nonuniform connection construction.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef TDICE_NONUNIFORM_SPATIAL_INDEX_H
#define TDICE_NONUNIFORM_SPATIAL_INDEX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "types.h"

typedef struct NonuniformSpatialIndex_t NonuniformSpatialIndex_t;

typedef struct NonuniformSpatialCache_t
{
    NonuniformSpatialIndex_t *Indexes;
    size_t Size;
    size_t Capacity;
    const ChipDimension_t (*Positions)[4];
} NonuniformSpatialCache_t;

/* A separate scratch vector is required for each querying thread. */
typedef struct NonuniformSpatialCandidates_t
{
    CellIndex_t *Values;
    size_t Size;
    size_t Capacity;
} NonuniformSpatialCandidates_t;

typedef struct NonuniformSpatialQueryStats_t
{
    uint64_t NodesVisited;
    uint64_t CellsTested;
} NonuniformSpatialQueryStats_t;

void nonuniform_spatial_cache_init
(
    NonuniformSpatialCache_t *cache,
    const ChipDimension_t (*positions)[4]
);
void nonuniform_spatial_cache_destroy (NonuniformSpatialCache_t *cache);

/* Builds on first use; call outside parallel regions. Returned pointers stay
 * valid until the next cache_get or cache_destroy. Ranges are half-open. */
const NonuniformSpatialIndex_t *nonuniform_spatial_cache_get
(
    NonuniformSpatialCache_t *cache,
    CellIndex_t start,
    CellIndex_t end
);

/* Returns ascending IDs >= minimum_id whose closed boxes intersect the
 * padded query box. Stats, when non-NULL, accumulate across calls. */
bool nonuniform_spatial_query
(
    const NonuniformSpatialIndex_t *index,
    const ChipDimension_t box[4],
    ChipDimension_t padding,
    CellIndex_t minimum_id,
    NonuniformSpatialCandidates_t *candidates,
    NonuniformSpatialQueryStats_t *stats
);
void nonuniform_spatial_candidates_destroy
(
    NonuniformSpatialCandidates_t *candidates
);

#endif
