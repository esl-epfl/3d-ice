"""
Analyze a GDS floorplan on a given layer.

Usage:
    python script.py <design.gds>  --layer n1   --max-x n2   --max-y n3   --max-iterations n4 --max-grid-area n5
    
        n1: layer index to extract floorplan
        n2: maximum x coordinate of the plane
        n3: maximum y coordinate of the plane
        n4: maximum iteration number (default value is 10)
        n5: maximum grid area (unit: um^2)
        (more details in user_guide page 42)

The script:
  • loads the GDS file and extracts polygons/paths on the chosen layer  
  • recursively subdivides a user-defined bounding box into smaller regions  
  • computes the area overlap ratio of each region with the layout  
  • writes results to: gds_layer<LAYER>.txt
  
"""


import gdspy
import numpy as np
from shapely.geometry import Polygon, MultiPolygon, box
from shapely.ops import unary_union
import rtree
import time
from pathlib import Path
import argparse
#import matplotlib.pyplot as plt
#import matplotlib.patches as patches
from collections import deque


# pase input arguments
parser = argparse.ArgumentParser(description="Process a GDS floorplan and export region stats.")
parser.add_argument("gds", type=Path, help="Path to the .gds file to parse")
parser.add_argument("--layer", type=int, help="GDS layer to process.")
parser.add_argument("--max-iterations", type=int, default=10, dest="max_iterations",
                    help="Max recursion depth for subdivision.")
parser.add_argument("--max-grid-area", type=float, default=0.01, dest="max_grid_area",
                    help="Stop subdividing when region area <= this value.")
parser.add_argument("--max-x", type=float, dest="max_x",
                    help="Bounding box max X (min X is fixed at 0).")
parser.add_argument("--max-y", type=float, dest="max_y",
                    help="Bounding box max Y (min Y is fixed at 0).")
args = parser.parse_args()

# Load the GDS file
gds_path = args.gds.expanduser()
if not gds_path.exists():
    parser.error(f"GDS file not found: {gds_path}")
if gds_path.suffix.lower() not in {".gds", ".gdsii", ".gds2"}:
    print(f"Warning: expected a .gds/.gdsii file, got {gds_path.suffix}")
gds_file_path = str(gds_path.resolve())
gds_data = gdspy.GdsLibrary()
gds_data.read_gds(gds_file_path)

# Function to merge polygons by layer and return merged shapes
def merge_polygons(polygons):
    shapely_polygons = [Polygon(poly) for poly in polygons]
    merged_shape = unary_union(shapely_polygons)
    # Handle MultiPolygon and single Polygon cases
    if isinstance(merged_shape, Polygon):
        return [merged_shape]
    elif isinstance(merged_shape, MultiPolygon):
        return list(merged_shape.geoms)
    else:
        raise TypeError("Unexpected geometry type after merging polygons.")

# Construct an R-tree spatial index for polygons
def build_spatial_index(polygons):
    spatial_index = rtree.index.Index()
    for idx, poly in enumerate(polygons):
        spatial_index.insert(idx, poly.bounds)
    print(f"Total nodes (polygons) in the R-tree: {spatial_index.count((float('-inf'), float('-inf'), float('inf'), float('inf')))}")
    return spatial_index

# Function to visualize the R-tree structure
# Plot function to visualize spatial index (R-tree) entries
# def plot_rtree(index, ax):
#     found_entries = False
#     all_bounds = []  # Track all bounds to determine plot limits

#     # Loop through all entries in the R-tree
#     for entry in index.intersection((float('-inf'), float('-inf'), float('inf'), float('inf')), objects=True):
#         found_entries = True
#         min_x, min_y, max_x, max_y = entry.bbox
#         all_bounds.append((min_x, min_y, max_x, max_y))  # Collect bounds for setting axis limits

#         # Draw a larger rectangle for better visibility
#         rect = patches.Rectangle((min_x, min_y), max_x - min_x, max_y - min_y, 
#                                  linewidth=2, edgecolor='red', facecolor='none', alpha=0.5)
#         ax.add_patch(rect)

#     # Set axis limits based on the bounding box of all entries
#     if found_entries:
#         all_x = [b[0] for b in all_bounds] + [b[2] for b in all_bounds]
#         all_y = [b[1] for b in all_bounds] + [b[3] for b in all_bounds]
#         ax.set_xlim(min(all_x) - 10, max(all_x) + 10)
#         ax.set_ylim(min(all_y) - 10, max(all_y) + 10)
#     else:
#         print("No entries found in the spatial index to plot.")

# Retrieve only polygons relevant to the specified region using the spatial index
def get_relevant_polygons(region, spatial_index, polygons):
    relevant_indices = list(spatial_index.intersection(region.bounds))
    return [polygons[idx] for idx in relevant_indices if region.intersects(polygons[idx])]

# Recursive subdivision function with early exits
def subdivide_region(min_x, min_y, max_x, max_y, polygons, spatial_index, iteration=0, max_iterations=8, threshold1=0.98, threshold2=0.02, max_grid_area = 100, element_index=1, output_file=None):
    region = box(min_x, min_y, max_x, max_y)
    region_area = region.area
    
    # Get relevant polygons using spatial index
    relevant_polygons = get_relevant_polygons(region, spatial_index, polygons)
    
    # Calculate overlap area with relevant polygons
    total_overlap_area = sum(region.intersection(poly).area for poly in relevant_polygons if region.intersects(poly))
    area_ratio = total_overlap_area / region_area if region_area > 0 else 0

    # Output the region information even if no relevant polygons
    if iteration >= max_iterations or ((area_ratio <= threshold2 or area_ratio >= threshold1) and region_area <= max_grid_area):
        output_file.write(
            f"Element {element_index}:\n"
            f" Region Position: ({min_x}, {min_y}), Size: ({max_x - min_x}, {max_y - min_y})\n"
            f" Overlapped Area Ratio: {area_ratio:.2f}\n\n"
        )
        return element_index + 1  # Increment element index for the next region

    # Recursive subdivision into quadrants
    mid_x = (min_x + max_x) / 2
    mid_y = (min_y + max_y) / 2
    element_index = subdivide_region(min_x, min_y, mid_x, mid_y, polygons, spatial_index, iteration + 1, max_iterations, threshold1, threshold2, max_grid_area, element_index, output_file)
    element_index = subdivide_region(mid_x, min_y, max_x, mid_y, polygons, spatial_index, iteration + 1, max_iterations, threshold1, threshold2, max_grid_area, element_index, output_file)
    element_index = subdivide_region(min_x, mid_y, mid_x, max_y, polygons, spatial_index, iteration + 1, max_iterations, threshold1, threshold2, max_grid_area, element_index, output_file)
    element_index = subdivide_region(mid_x, mid_y, max_x, max_y, polygons, spatial_index, iteration + 1, max_iterations, threshold1, threshold2, max_grid_area, element_index, output_file)
    return element_index

# User-defined layer to process
specified_layer = args.layer
max_iterations = args.max_iterations
threshold1 = 0.98
threshold2 = 0.02
# max grid area
max_grid_area = args.max_grid_area

# Process only the specified layer, including paths
layer_data = {}
for cell_name, cell in gds_data.cells.items():
    polygons = cell.get_polygons(by_spec=True)
    paths = cell.get_paths()
    for (layer, datatype), poly_list in polygons.items():
        if layer == specified_layer:
            if layer not in layer_data:
                layer_data[layer] = []
            layer_data[layer].extend(poly_list)
    # Filter paths by layer manually
    for path in paths:
        if path.layers[0] == specified_layer:  # Check if path layer matches
            if specified_layer not in layer_data:
                layer_data[specified_layer] = []
            # Use get_polygons() for FlexPath objects
            if isinstance(path, gdspy.FlexPath):
                layer_data[specified_layer].extend(path.get_polygons())
            elif isinstance(path, gdspy.Path):
                path_poly = path.to_polygon()  # Convert Path to Polygon
                layer_data[specified_layer].append(path_poly)
            else:
                print(f"Unsupported path type: {type(path)}")

# Use the spatial index functions to get nodes and depth information
if specified_layer in layer_data:
    start_time = time.time()
    merged_polygons = merge_polygons(layer_data[specified_layer])
    
    if merged_polygons:
        spatial_index = build_spatial_index(merged_polygons)

        #fig, ax = plt.subplots(figsize=(10, 10))
        #plot_rtree(spatial_index, ax)
        #plt.xlabel("X")
        #plt.ylabel("Y")
        #plt.title("R-tree Spatial Index Visualization - Zoomed")
        #plt.show()
    else:
        print("No polygons found after merging for the specified layer.")
    
    # # Get the bounding box of the entire merged polygons set
    # min_x, min_y, max_x, max_y = merged_polygons[0].bounds
    # min_x = min_x if min_x > 0 else float('inf')
    # min_y = min_y if min_y > 0 else float('inf')
    
    # for poly in merged_polygons[1:]:
    #     poly_min_x, poly_min_y, poly_max_x, poly_max_y = poly.bounds
    #     if poly_min_x > 0:
    #         min_x = min(min_x, poly_min_x)
    #     if poly_min_y > 0:
    #         min_y = min(min_y, poly_min_y)
    #     max_x = max(max_x, poly_max_x)
    #     max_y = max(max_y, poly_max_y)
        
    min_x = 0
    min_y = 0
    max_x = args.max_x
    max_y = args.max_y
        
    output_path = f"gds_layer{specified_layer}.txt"
    with open(output_path, "w") as f:
        f.write(f"Floorplan Information for Layer {specified_layer}\n{'=' * 40}\n\n")
        subdivide_region(min_x, min_y, max_x, max_y, merged_polygons, spatial_index, max_iterations=max_iterations, threshold1=threshold1, threshold2=threshold2, max_grid_area = max_grid_area, output_file=f)
    
    # End timing and output general time information
    end_time = time.time()
    print(f"Time taken to process and output the floorplan: {end_time - start_time:.2f} seconds")
else:
    print(f"Layer {specified_layer} not found in the GDS file.")