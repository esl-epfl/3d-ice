/* Private shared-cache entry point, also used by connection-only benchmarks.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef TDICE_NONUNIFORM_CONNECTIONS_H
#define TDICE_NONUNIFORM_CONNECTIONS_H

#include "dimensions.h"

/* Replace the graph and update matrix NNZ only after both stages succeed.
 * Layer ends are cumulative, followed by an optional spreader end (type 4).
 */
Error_t build_nonuniform_connections
(
    CellIndex_t *layer_ends,
    CellIndex_t *layer_types,
    ChipDimension_t (*positions)[4],
    Dimensions_t *dimensions
);

#endif
