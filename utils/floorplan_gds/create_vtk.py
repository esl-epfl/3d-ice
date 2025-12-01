"""
Convert floorplan region stats (.txt) into a VTK POLYDATA file.

Usage:
    python script.py *.txt

The script:
  • reads rectangles in the "Element N: Region Position..., Size..., Overlapped Area Ratio..." format
  • writes a .vtk file with one quad per region and the area ratio as cell data.

"""

from __future__ import annotations
import argparse
import re
from pathlib import Path

# Regex robust to spaces and scientific notation
NUM = r"[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?"
BLOCK_RE = re.compile(
    rf"""
    Element\s+\d+\s*:\s*            # 'Element N:'
    \n\s*Region\s+Position:\s*\(\s*({NUM})\s*,\s*({NUM})\s*\)\s*,\s*
       Size:\s*\(\s*({NUM})\s*,\s*({NUM})\s*\)\s*     # (x,y), Size (w,h)
    \n\s*Overlapped\s+Area\s+Ratio:\s*({NUM})           # ratio
    """,
    re.VERBOSE | re.MULTILINE,
)

def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="Convert floorplan rectangles text to VTK POLYDATA.")
    p.add_argument("file_path", type=Path, help="Path to floorplan .txt (e.g., floorplan_layer67.txt)")
    return p.parse_args()

def read_blocks(text: str):
    """Yield (x, y, w, h, ratio) for each Element block found."""
    for m in BLOCK_RE.finditer(text):
        x, y, w, h, r = map(float, m.groups())
        yield x, y, w, h, r

def write_vtk(out_path: Path, rects: list[tuple[float, float, float, float]], ratios: list[float]) -> None:
    n = len(rects)
    if n == 0:
        raise ValueError("No rectangles parsed; check the input file format.")
    num_vertices = n * 4
    num_polygons = n

    with out_path.open("w", encoding="utf-8") as f:
        # Header
        f.write("# vtk DataFile Version 2.0\n")
        f.write("Rectangle floorplan data\n")
        f.write("ASCII\n")
        f.write("DATASET POLYDATA\n")

        # Points
        f.write(f"POINTS {num_vertices} float\n")
        for (x, y, w, h) in rects:
            f.write(f"{x} {y} 0\n")          # bottom-left
            f.write(f"{x + w} {y} 0\n")      # bottom-right
            f.write(f"{x + w} {y + h} 0\n")  # top-right
            f.write(f"{x} {y + h} 0\n")      # top-left

        # Polygons: each uses 4 consecutive points
        f.write(f"POLYGONS {num_polygons} {num_polygons * 5}\n")
        for i in range(num_polygons):
            base = i * 4
            f.write(f"4 {base} {base + 1} {base + 2} {base + 3}\n")

        # Cell data (area ratio)
        f.write(f"CELL_DATA {num_polygons}\n")
        f.write("SCALARS Area_Ratio float 1\n")
        f.write("LOOKUP_TABLE default\n")
        for r in ratios:
            f.write(f"{r}\n")

def main() -> None:
    args = parse_args()
    in_path: Path = args.file_path.expanduser().resolve()
    if not in_path.exists():
        raise FileNotFoundError(f"Input not found: {in_path}")

    # Derive output path by changing extension to .vtk
    out_path = in_path.with_suffix(".vtk")

    text = in_path.read_text(encoding="utf-8", errors="ignore")
    rects, ratios = [], []
    for x, y, w, h, r in read_blocks(text):
        rects.append((x, y, w, h))
        ratios.append(r)

    write_vtk(out_path, rects, ratios)
    print(f"VTK file generated: {out_path}")

if __name__ == "__main__":
    main()
