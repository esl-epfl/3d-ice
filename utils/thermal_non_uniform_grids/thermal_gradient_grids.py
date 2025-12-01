"""
Refine a floorplan using temperature gradients from a prior coarse-grained uniform simulation.

Usage:
    python script.py  --xyaxis path_toxyaxis  --tem path_to_tem  --floorplan path_to_floorplan  --percentage n1  
                      --min-grid-x n2  --min-grid-y n3  --max-grid-x n4  --max-grid-y n5   --out-flp  path_to_output

    path_toxyaxis:  path to initial grid coordinates
    path_to_tem: path to initial grid temperature
    path_to_floorplan: path to the floorplan file requires refinements 
    n1: between 0-100 that control the discretizaion level
    n2: minimum grid size in x-direction (unit: um)
    n3: minimum grid size in y-direction (unit: um)
    n4: maximum grid size in x-direction (unit: um)
    n5: maximum grid size in y-direction (unit: um)
    path_to_output: path to generated non-uniform floorplan file
    (more details in user_guide page 44)

The script:
  • Computes |∇T|, refines local grid sizes between user-given min/max, and writes an updated floorplan.

"""

import re
import math
from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.colors import Normalize
import argparse
import sys

def parse_args():
    p = argparse.ArgumentParser(
        description="Transform inputs, compute gradients, refine grid, plot, and write .flp."
    )
    # Required file paths
    p.add_argument("--xyaxis", required=True, help="Path to the xy coordinates for the initial simulation")
    p.add_argument("--tem", required=True, help="Path to the temperature for the initial simulation")
    p.add_argument("--floorplan", required=True, help="Path to the floorplan template.flp")
    # Optional: output .flp filename
    p.add_argument("--out-flp", default="non_uniform_grids.flp",
                   help="Output floorplan filename (default: %(default)s)")
    # Refinement knobs (defaults match your MATLAB)
    p.add_argument("--percentage", type=float, default=10.0,
                   help="Percentile used as P_ref in refinement (0–100). Default: %(default)s")
    p.add_argument("--min-grid-x", type=float,
                   help="Minimum grid size in x (µm).")
    p.add_argument("--min-grid-y", type=float,
                   help="Minimum grid size in y (µm).")
    p.add_argument("--max-grid-x", type=float,
                   help="Maximum grid size in x (µm).")
    p.add_argument("--max-grid-y", type=float, 
                   help="Maximum grid size in y (µm).")
    # (Optional) skip plots if you want headless runs
    p.add_argument("--no-plots", action="store_true", help="Disable plotting windows")

    args = p.parse_args()

    # Basic validation
    if not (0.0 <= args.percentage <= 100.0):
        p.error("--percentage must be in [0, 100].")
    if args.min_grid_x <= 0 or args.min_grid_y <= 0:
        p.error("--min-grid-x and --min-grid-y must be > 0.")
    if args.max_grid_x <= 0 or args.max_grid_y <= 0:
        p.error("--max-grid-x and --max-grid-y must be > 0.")
    if args.min_grid_x > args.max_grid_x:
        p.error("--min-grid-x cannot be greater than --max-grid-x.")
    if args.min_grid_y > args.max_grid_y:
        p.error("--min-grid-y cannot be greater than --max-grid-y.")

    return args


def load_xyaxis(xyaxis_file: str):
    xyaxis = np.loadtxt(xyaxis_file)
    x_coords = xyaxis[:, 0]
    y_coords = xyaxis[:, 1]
    dx = xyaxis[:, 2]
    dy = xyaxis[:, 3]
    return x_coords, y_coords, dx, dy


def load_temperature_first_timestep(temperature_file: str):
    """
    Load the first time step of temperatures from `temperature_file`, while
    skipping any header/comment/non-numeric lines (e.g., lines starting with '%').

    Returns a 1D NumPy array of temps.
    """
    lines_numeric = []
    with open(temperature_file, "r", encoding="utf-8-sig") as f:
        for raw in f:
            s = raw.strip()
            if not s:
                continue
            # Skip typical comments / headers
            if s.startswith("%") or s.startswith("#"):
                continue
            # Skip lines containing letters (non-numeric headers)
            if re.search(r"[A-Za-z]", s):
                continue
            # Parse floats from the line
            vals = np.fromstring(s, sep=" ")
            if vals.size:
                lines_numeric.append(vals)

    if not lines_numeric:
        raise ValueError(f"No numeric data found in {temperature_file}")

    # If multiple numeric lines have equal length, treat as 2D; else flatten.
    lens = {len(v) for v in lines_numeric}
    if len(lines_numeric) > 1 and len(lens) == 1:
        arr = np.vstack(lines_numeric)          # shape: (T, N)
    else:
        arr = np.concatenate(lines_numeric)     # shape: (N,)

    # Match your original intent: use first time step if 2D, else the 1D vector.
    temps = arr[0, :] if arr.ndim == 2 else arr
    return temps


def sort_and_reshape_grid(x_coords, y_coords, dx, dy, temps):
    # Compute centers (assuming lower-left corner input)
    center_x = x_coords + dx / 2.0
    center_y = y_coords + dy / 2.0

    # Stack columns to mirror MATLAB 'data' array
    data = np.column_stack([x_coords, y_coords, dx, dy, center_x, center_y, temps])

    # Sort by center_y (primary), then center_x (secondary)
    # np.lexsort uses last key as primary
    sort_idx = np.lexsort((data[:, 4], data[:, 5]))
    data_sorted = data[sort_idx, :]

    sorted_llx = data_sorted[:, 0]
    sorted_lly = data_sorted[:, 1]
    sorted_dx = data_sorted[:, 2]
    sorted_dy = data_sorted[:, 3]
    sorted_center_x = data_sorted[:, 4]
    sorted_center_y = data_sorted[:, 5]
    sorted_temps = data_sorted[:, 6]

    # Determine grid dims
    unique_y, inv_y = np.unique(sorted_center_y, return_inverse=True)
    Ny = unique_y.size
    Nx = np.sum(inv_y == 0)

    # Verify rectangular grid
    for k in range(Ny):
        if np.sum(inv_y == k) != Nx:
            raise ValueError(f"The grid is not rectangular. Row {k+1} has a different number of cells.")

    # Reshape into (Ny, Nx) using row-major order (sorted by row then col)
    LLX_matrix = sorted_llx.reshape(Ny, Nx)
    LLY_matrix = sorted_lly.reshape(Ny, Nx)
    DX_matrix = sorted_dx.reshape(Ny, Nx)
    DY_matrix = sorted_dy.reshape(Ny, Nx)
    CenterX_matrix = sorted_center_x.reshape(Ny, Nx)
    CenterY_matrix = sorted_center_y.reshape(Ny, Nx)
    T_matrix = sorted_temps.reshape(Ny, Nx)

    return (LLX_matrix, LLY_matrix, DX_matrix, DY_matrix,
            CenterX_matrix, CenterY_matrix, T_matrix, Nx, Ny)


def compute_gradient(T_matrix, CenterX_matrix, CenterY_matrix, Nx, Ny):
    # Grid spacing from unique values along axes
    if Nx > 1:
        spacingX = np.mean(np.diff(np.unique(CenterX_matrix[0, :])))
    else:
        spacingX = 1.0

    if Ny > 1:
        spacingY = np.mean(np.diff(np.unique(CenterY_matrix[:, 0])))
    else:
        spacingY = 1.0

    # numpy.gradient returns [dT/dy, dT/dx] when given spacing for (axis0, axis1)
    dT_dy, dT_dx = np.gradient(T_matrix, spacingY, spacingX)
    grad_mag = np.sqrt(dT_dx ** 2 + dT_dy ** 2)
    return grad_mag, spacingX, spacingY


def plot_gradient_rectangles(LLX_matrix, LLY_matrix, DX_matrix, DY_matrix, grad_mag):
    fig, ax = plt.subplots()
    ax.set_aspect('equal', adjustable='box')

    # Limits from lower-left to upper-right of all rectangles
    x_min = np.min(LLX_matrix)
    x_max = np.max(LLX_matrix + DX_matrix)
    y_min = np.min(LLY_matrix)
    y_max = np.max(LLY_matrix + DY_matrix)
    ax.set_xlim([x_min, x_max])
    ax.set_ylim([y_min, y_max])

    # Colormap setup
    cmap = plt.get_cmap('jet', 256)
    num_colors = 256
    min_grad = float(np.min(grad_mag))
    max_grad = float(np.max(grad_mag))
    denom = (max_grad - min_grad) + np.finfo(float).eps

    Ny, Nx = grad_mag.shape
    for i in range(Ny):
        for j in range(Nx):
            norm_val = (grad_mag[i, j] - min_grad) / denom
            # Optional quantization to emulate MATLAB indexing
            color_idx = max(0, min(num_colors - 1, int(round(norm_val * (num_colors - 1)))))
            face_color = cmap(color_idx)

            rect_x = LLX_matrix[i, j]
            rect_y = LLY_matrix[i, j]
            rect_w = DX_matrix[i, j]
            rect_h = DY_matrix[i, j]

            ax.add_patch(Rectangle((rect_x, rect_y), rect_w, rect_h,
                                   facecolor=face_color, edgecolor='none'))

    # Colorbar with proper normalization
    norm = Normalize(vmin=min_grad, vmax=max_grad)
    sm = plt.cm.ScalarMappable(norm=norm, cmap=cmap)
    sm.set_array([])
    cbar = plt.colorbar(sm, ax=ax)
    cbar.ax.tick_params(labelsize=12)
    cbar.set_label('Temperature gradient (K/µm)', fontsize=14, fontweight='bold')

    ax.set_xlabel('X (µm)')
    ax.set_ylabel('Y (µm)')
    ax.grid(True)
    plt.title('2D Gradient Magnitude Distribution')
    plt.tight_layout()
    plt.show()


def parse_floorplan(filename: str):
    """
    Parse 'readin_floorplan.flp' into a list of dicts with keys:
      name           : element name, e.g. 'unit_0', 'Hotspot0', ...
      position       : np.array([x0, y0])
      dimension      : np.array([Lx, Ly])
      discretization : np.array([nx, ny]) or None
      material       : str or None
      power          : list[float] or None  <-- now a list, not a single float
    """
    data = []
    current = None
    
    pat_elem  = re.compile(r'^\s*([A-Za-z0-9_.\-]+)\s*:', re.IGNORECASE)
    pat_pos   = re.compile(r'position\s+([-\d\.]+),\s*([-\d\.]+)\s*;?', re.IGNORECASE)
    pat_dim   = re.compile(r'dimension\s+([-\d\.]+),\s*([-\d\.]+)\s*;?', re.IGNORECASE)
    pat_disc  = re.compile(r'discretization\s+([-\d\.]+),\s*([-\d\.]+)\s*;?', re.IGNORECASE)
    # capture everything after "power values" up to the ';'
    pat_power = re.compile(r'power\s+values\s+(.+?);?$', re.IGNORECASE)
    pat_mat   = re.compile(r'material\s+(\S+)\s*;?', re.IGNORECASE)

    with open(filename, 'r') as f:
        for raw in f:
            line = raw.strip()
            if not line:
                continue

            # New element block?
            m_elem = pat_elem.match(line)
            if m_elem:
                if current is not None:
                    data.append(current)
                current = {
                    'name': m_elem.group(1),
                    'position': None,
                    'dimension': None,
                    'discretization': None,
                    'material': None,
                    'power': None,   # will be list[float]
                    'grid_x': None,
                    'grid_y': None,
                    'num_grids_x': None,
                    'num_grids_y': None,
                    'total_cells': None,
                }
                continue

            if current is None:
                continue

            m = pat_pos.search(line)
            if m:
                current['position'] = np.array(
                    [float(m.group(1)), float(m.group(2))],
                    dtype=float
                )
                continue

            m = pat_dim.search(line)
            if m:
                current['dimension'] = np.array(
                    [float(m.group(1)), float(m.group(2))],
                    dtype=float
                )
                continue

            m = pat_disc.search(line)
            if m:
                current['discretization'] = np.array(
                    [float(m.group(1)), float(m.group(2))],
                    dtype=float
                )
                continue

            m = pat_mat.search(line)
            if m:
                current['material'] = m.group(1)
                continue

            m = pat_power.search(line)
            if m:
                # Parse all numbers in the remainder of the line
                num_str = m.group(1).strip()
                # Split by comma or whitespace
                raw_vals = re.split(r'[,\s]+', num_str)
                vals = [float(v) for v in raw_vals if v]
                if vals:
                    current['power'] = vals
                continue

    if current is not None:
        data.append(current)

    if not data:
        raise ValueError(
            f"No floorplan elements found in {filename}. "
            "Expected lines like 'Hotspot0 :' or 'unit_0 :'."
        )

    for i, d in enumerate(data):
        if d['position'] is None or d['dimension'] is None:
            raise ValueError(f"Missing position/dimension for element index {i} ({d.get('name')})")

    return data


def compute_avg_gradient_per_element(data, LLX_matrix, LLY_matrix, grad_mag):
    Ny, Nx = LLX_matrix.shape
    avgGrad = np.zeros(len(data), dtype=float)
    avgGrad[:] = np.nan

    for i, elem in enumerate(data):
        x_min = elem['position'][0]
        y_min = elem['position'][1]
        x_max = x_min + elem['dimension'][0]
        y_max = y_min + elem['dimension'][1]

        mask = ((LLX_matrix >= x_min) & (LLX_matrix <= x_max) &
                (LLY_matrix >= y_min) & (LLY_matrix <= y_max))

        cellGradients = grad_mag[mask]
        if cellGradients.size == 0:
            avgGrad[i] = np.nan
        else:
            avgGrad[i] = np.nanmean(cellGradients)

    # Normalize to [0,1]
    minGrad = np.nanmin(avgGrad)
    maxGrad = np.nanmax(avgGrad)
    normAvgGrad = (avgGrad - minGrad) / (maxGrad - minGrad + np.finfo(float).eps)
    return avgGrad, normAvgGrad


def refine_discretization(data, normAvgGrad,
                          alpha=1.0, percentage=10,
                          desired_min_grid_x=77.0, desired_min_grid_y=77.0,
                          desired_max_grid_x=200.0, desired_max_grid_y=200.0):
    # Reference percentile
    P_ref = float(np.nanpercentile(normAvgGrad, percentage))
    eps = 1e-6

    total_cells = 0
    for i, elem in enumerate(data):
        h0_x = float(elem['dimension'][0])
        h0_y = float(elem['dimension'][1])

        metric = float(normAvgGrad[i])

        target_x = h0_x * (P_ref / (metric + eps)) ** alpha
        target_y = h0_y * (P_ref / (metric + eps)) ** alpha

        refined_x = min(max(target_x, desired_min_grid_x), desired_max_grid_x)
        refined_y = min(max(target_y, desired_min_grid_y), desired_max_grid_y)

        elem['grid_x'] = refined_x
        elem['grid_y'] = refined_y

        # Initial counts
        nx = int(math.ceil(h0_x / refined_x))
        ny = int(math.ceil(h0_y / refined_y))

        # Enforce square cells by setting both to the larger count
        nx = max(nx, ny)
        ny = nx

        elem['num_grids_x'] = nx
        elem['num_grids_y'] = ny
        elem['total_cells'] = nx * ny

        total_cells += elem['total_cells']

    max_num_grids_x = max(d['num_grids_x'] for d in data)
    max_num_grids_y = max(d['num_grids_y'] for d in data)

    print(f"Total number of grid cells: {total_cells}")
    print(f"Maximum num_grids_x among elements: {max_num_grids_x}")
    print(f"Maximum num_grids_y among elements: {max_num_grids_y}")

    return total_cells, max_num_grids_x, max_num_grids_y


def plot_floorplan_grids(data):
    fig, ax = plt.subplots()
    ax.set_aspect('equal', adjustable='box')
    ax.grid(True)
    ax.set_xlabel('X (µm)')
    ax.set_ylabel('Y (µm)')

    # Draw each element and its internal grid
    for elem in data:
        x0, y0 = elem['position']
        Lx, Ly = elem['dimension']
        nx = elem['num_grids_x']
        ny = elem['num_grids_y']

        # Outer rectangle
        ax.add_patch(Rectangle((x0, y0), Lx, Ly, fill=False, edgecolor='k', linewidth=1.5))

        # Internal grid lines
        dx = Lx / nx
        dy = Ly / ny

        # Vertical lines
        for j in range(1, nx):
            xline = x0 + j * dx
            ax.plot([xline, xline], [y0, y0 + Ly])

        # Horizontal lines
        for k in range(1, ny):
            yline = y0 + k * dy
            ax.plot([x0, x0 + Lx], [yline, yline])

    # Adjust limits
    positions = np.array([elem['position'] for elem in data], dtype=float)
    dims = np.array([elem['dimension'] for elem in data], dtype=float)
    x_vals = positions[:, 0]
    y_vals = positions[:, 1]
    x_max = np.max(x_vals + dims[:, 0])
    y_max = np.max(y_vals + dims[:, 1])
    ax.set_xlim([np.min(x_vals) - 1.0, x_max + 1.0])
    ax.set_ylim([np.min(y_vals) - 1.0, y_max + 1.0])

    plt.title('Floorplan Elements and Grids')
    plt.tight_layout()
    plt.show()


def write_floorplan_output(data, output_filename='non_uniform_grids.flp'):
    with open(output_filename, 'w') as f:
        for i, elem in enumerate(data):
            name = elem.get('name', f'unit_{i}')
            f.write(f'{name} :\n\n')
            f.write(f'  position    {elem["position"][0]:.6f}, {elem["position"][1]:.6f} ;\n')
            f.write(f'  dimension   {elem["dimension"][0]:.6f}, {elem["dimension"][1]:.6f} ;\n')

            material = elem.get("material")
            if material is not None:
                f.write(f'  material {material} ;\n')

            f.write(f'  discretization {elem["num_grids_x"]}, {elem["num_grids_y"]} ;\n')

            # elem["power"] is now a list[float] or None
            power_vals = elem.get("power")
            if not power_vals:
                # if missing, keep old behavior: one zero value
                power_vals = [0.0]

            power_str = ", ".join(f"{p:.12f}" for p in power_vals)
            f.write(f'  power values  {power_str} ;\n\n')

    print(f'Data has been written to {output_filename}')


def main(args):

    # --- Check files exist ---
    for fname in [args.xyaxis, args.tem, args.floorplan]:
        if not Path(fname).exists():
            raise FileNotFoundError(f"Required file not found: {fname}")
        
    # --- Load Data ---
    x_coords, y_coords, dx, dy = load_xyaxis(args.xyaxis)
    temps = load_temperature_first_timestep(args.tem)

    # --- Prepare Grid ---
    (LLX_matrix, LLY_matrix, DX_matrix, DY_matrix,
     CenterX_matrix, CenterY_matrix, T_matrix, Nx, Ny) = sort_and_reshape_grid(
        x_coords, y_coords, dx, dy, temps
    )

    # --- Gradient ---
    grad_mag, spacingX, spacingY = compute_gradient(T_matrix, CenterX_matrix, CenterY_matrix, Nx, Ny)

    # --- Plot 2D gradient magnitude via colored rectangles ---
    if not args.no_plots:
        plot_gradient_rectangles(LLX_matrix, LLY_matrix, DX_matrix, DY_matrix, grad_mag)

    # --- Parse floorplan and compute average gradient per element ---
    data = parse_floorplan(args.floorplan)
    avgGrad, normAvgGrad = compute_avg_gradient_per_element(
        data, LLX_matrix, LLY_matrix, grad_mag
    )

    # --- Compute grid discretization per element (local refinement) ---
    total_cells, max_nx, max_ny = refine_discretization(
        data, normAvgGrad,
        alpha=1.0, 
        percentage=args.percentage,
        desired_min_grid_x=args.min_grid_x,
        desired_min_grid_y=args.min_grid_y,
        desired_max_grid_x=args.max_grid_x,
        desired_max_grid_y=args.max_grid_y
    )

    # --- Plot floorplan grids ---
    if not args.no_plots:
        plot_floorplan_grids(data)

    # --- Output .flp file ---
    write_floorplan_output(data, args.out_flp)


if __name__ == '__main__':
    # Ensure files exist, then run
    args = parse_args()
    try:
        main(args)
    except Exception as e:
        print(f"ERROR: {e}", file=sys.stderr)
        sys.exit(1)
