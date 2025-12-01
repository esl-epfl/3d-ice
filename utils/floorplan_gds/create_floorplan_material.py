"""
Generate 3D-ICE-style floorplan.txt and material.txt from GDS region stats.

Usage:
    python script.py  *.txt

The script converts a floorplan text file to:
  • floorplan.txt  (elements with materials, positions, etc.)
  • material.txt   (unique materials with thermal properties)

"""

from __future__ import annotations
import argparse
import math
import re
from pathlib import Path

# -----------------------------
# Thermal characteristics for polygons
k_Polygon = 385.0
Cp_Polygon = 3449600.0

# Thermal characteristics for substrate
k_Substrate = 1.5
Cp_Substrate = 1540000.0

Label_start = 0
# The number of generated material types
Num_of_material = 100

# -----------------------------
#magnify the dimension of the floorplan element in x and y direction simulataneously
magnify = 1.0

discretization_x = 1
discretization_y = 1
power_value = 0.0

# Regex to parse blocks:
#   Element N:
#     Region Position: (x, y), Size: (w, h)
#     Material Area Ratio: r
NUM = r"[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?"
BLOCK_RE = re.compile(
    rf"""
    Element\s+\d+\s*:\s*\n
    \s*Region\s+Position:\s*\(\s*({NUM})\s*,\s*({NUM})\s*\)\s*,\s*
       Size:\s*\(\s*({NUM})\s*,\s*({NUM})\s*\)\s*\n
    \s*Overlapped\s+Area\s+Ratio:\s*({NUM})
    """,
    re.VERBOSE | re.MULTILINE,
)

def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="Generate floorplan.txt and material.txt from a GDS rectangles text file.")
    p.add_argument("file_path", type=Path, help="Path to the input rectangles text (e.g., gds_layer67.txt)")
    return p.parse_args()

def read_rectangles(text: str):
    """Yield (x, y, w, h, ratio) for each 'Element' block."""
    for m in BLOCK_RE.finditer(text):
        x, y, w, h, r = map(float, m.groups())
        yield x, y, w, h, r

def write_floorplan(out_path: Path, rects, ratios):
    """Write floorplan.txt with discretization and material mapping."""
    with out_path.open("w", encoding="utf-8") as f:
        for i, ((x, y, w, h), r) in enumerate(zip(rects, ratios), start=1):
            # MATLAB: ratio_index = floor(r*Num) + 1; if ==1 -> 0; clamp to Num
            ratio_index = int(math.floor(r * Num_of_material)) + 1
            if ratio_index == 1:
                ratio_index = 0
            if ratio_index > Num_of_material:
                ratio_index = Num_of_material
            # offset the label
            material_id = Label_start + ratio_index
            material_name = f"GDS_mat{material_id}"

            f.write(f"Element{i} :\n")
            f.write(f"  position    {x * magnify:.12f}, {y * magnify:.12f} ;\n")
            f.write(f"  dimension   {w * magnify:.12f}, {h * magnify:.12f} ;\n")
            f.write(f"  material {material_name};\n")
            f.write(f"  discretization {discretization_x}, {discretization_y} ;\n")
            f.write(f"  power values {power_value:.12f} ;\n\n")

def write_materials(out_path: Path, ratios):
    """Write material.txt with thermal properties for unique materials."""
    # Unique material indices per MATLAB:
    # unique(floor(r*Num)+1), then keep <= Num_of_material
    raw_indices = {int(math.floor(r * Num_of_material)) + 1 for r in ratios}
    unique_indices = sorted(idx for idx in raw_indices if idx <= Num_of_material)

    with out_path.open("w", encoding="utf-8") as f:
        for idx in unique_indices:
            base_idx = 0 if idx == 1 else idx    # physics index (0..Num_of_material)
            P_GE = base_idx / Num_of_material
            sqrt_P_GE = math.sqrt(P_GE)

            # Thermal property calculations (as in MATLAB)
            k_eq_xy_num = sqrt_P_GE * k_Polygon * k_Substrate
            k_eq_xy_den = (1 - sqrt_P_GE) * k_Polygon + sqrt_P_GE * k_Substrate
            k_eq_x = ((1 - sqrt_P_GE) * k_Substrate + (k_eq_xy_num / k_eq_xy_den)) / 1e6
            k_eq_y = k_eq_x
            k_eq_z = (P_GE * k_Polygon + (1 - P_GE) * k_Substrate) / 1e6

            volumetric_heat_capacity = (P_GE * Cp_Polygon + (1 - P_GE) * Cp_Substrate) / 1e18

            material_id = Label_start + base_idx
            material_name = f"GDS_mat{material_id}"

            f.write(f"material {material_name} :\n")
            f.write(f"   thermal conductivity     {k_eq_x:.6e}, {k_eq_y:.6e}, {k_eq_z:.6e};\n")
            f.write(f"   volumetric heat capacity {volumetric_heat_capacity:.6e};\n\n")

def main():
    args = parse_args()
    in_path: Path = args.file_path.expanduser().resolve()
    if not in_path.exists():
        raise FileNotFoundError(f"Input not found: {in_path}")

    text = in_path.read_text(encoding="utf-8", errors="ignore")
    rects, ratios = [], []
    for x, y, w, h, r in read_rectangles(text):
        rects.append((x, y, w, h))
        ratios.append(r)

    if not rects:
        raise ValueError("No 'Element' blocks found. Check the input file format.")

    # Outputs in the same directory as the input (same names as MATLAB)
    out_dir = in_path.parent
    out_flp = out_dir / "floorplan.txt"
    out_mat = out_dir / "material.txt"

    write_floorplan(out_flp, rects, ratios)
    write_materials(out_mat, ratios)

    print(f"Wrote: {out_flp}")
    print(f"Wrote: {out_mat}")

if __name__ == "__main__":
    main()
