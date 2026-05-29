#
# Copyright (C) 2025 ETH Zurich and University of Bologna
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Author: Kai Zhu <kai.zhu@epfl.ch>

import argparse
import os
import re


SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DEFAULT_OUTPUT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
DEFAULT_FLOORPLAN_FILE = os.path.join(DEFAULT_OUTPUT_DIR, "floorplan_nopower.flp")
DEFAULT_STK_FILE = os.path.join(DEFAULT_OUTPUT_DIR, "example_transient_test_server.stk")


def format_number(value):
    return f"{value:.15g}"


def parse_floorplan_region(floorplan_file):
    with open(floorplan_file, "r") as f:
        floorplan = f.read()

    entry_re = re.compile(
        r"^\s*[^:\n]+\s*:\s*"
        r"\n\s*position\s+([^,]+),\s*([^;]+);"
        r"\s*\n\s*dimension\s+([^,]+),\s*([^;]+);",
        re.MULTILINE,
    )

    min_x = None
    min_y = None
    max_x = None
    max_y = None

    for match in entry_re.finditer(floorplan):
        x = float(match.group(1))
        y = float(match.group(2))
        width = float(match.group(3))
        height = float(match.group(4))

        min_x = x if min_x is None else min(min_x, x)
        min_y = y if min_y is None else min(min_y, y)
        max_x = x + width if max_x is None else max(max_x, x + width)
        max_y = y + height if max_y is None else max(max_y, y + height)

    if max_x is None or max_y is None:
        raise RuntimeError(f"No floorplan elements found in {floorplan_file}")

    return min_x, min_y, max_x, max_y


def get_floorplan_path_for_stk(floorplan_file, stk_file):
    stk_dir = os.path.dirname(os.path.abspath(stk_file))
    rel_path = os.path.relpath(os.path.abspath(floorplan_file), stk_dir)
    if not rel_path.startswith("."):
        rel_path = f"./{rel_path}"
    return rel_path


def stk_template(chip_length, chip_width, floorplan_path):
    return f"""material SILICON :
   thermal conductivity     1.30e-4 ;
   volumetric heat capacity 1.628e-12 ;

material BEOL :
   thermal conductivity     2.25e-6 ;
   volumetric heat capacity 2.175e-12 ;

top heat sink :
   heat transfer coefficient 1.0e-8 ;
   temperature               300 ;

dimensions :
   chip length {format_number(chip_length)}, width {format_number(chip_width)} ;
   cell length  50, width  50 ;
   non-uniform true;

layer PCB :
   height 50 ;
   material BEOL ;

die IC :
   source  50 SILICON ;
    

stack:
die     TOP_DIE         IC      floorplan "{floorplan_path}" ;
layer   CONN_TO_PCB     PCB ;

solver:
   transient step 0.02, slot 0.2 ;
   initial temperature 300.0 ;
   numofcores 8 ;   

output:
     Tflp( TOP_DIE,    "../results/output_top_die_flp_avg.txt",    average, slot );

"""


def roi2ice_stk(floorplan_file, stk_file):
    min_x, min_y, max_x, max_y = parse_floorplan_region(floorplan_file)

    if min_x != 0 or min_y != 0:
        print(f"Warning: floorplan region starts at ({min_x}, {min_y}); using max range for chip size.")

    floorplan_path = get_floorplan_path_for_stk(floorplan_file, stk_file)
    os.makedirs(os.path.dirname(os.path.abspath(stk_file)), exist_ok=True)

    with open(stk_file, "w") as f:
        f.write(stk_template(max_x, max_y, floorplan_path))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate 3D-ICE STK file from a floorplan file")
    parser.add_argument("floorplan_file", nargs="?", default=DEFAULT_FLOORPLAN_FILE, help="Path to floorplan file")
    parser.add_argument("stk_file", nargs="?", default=DEFAULT_STK_FILE, help="Output STK file")
    args = parser.parse_args()

    roi2ice_stk(args.floorplan_file, args.stk_file)
