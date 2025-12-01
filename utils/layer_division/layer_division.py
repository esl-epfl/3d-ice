#!/usr/bin/env python3

"""
Read conductances, validate format, then subdivide low-conductance layers.

Usage:
    python script.py *.txt  --i-max n

    *.txt: the path to the generated conductance_layer.txt
    n: denotes the maximum iteration number
    (more details in user_guide page 41)

The script:
  • output floorplan.txt (geometry/power) and material.txt (effective thermal properties).
"""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import argparse
import re
import sys
from typing import Iterable, Iterator, List, Tuple


# ---------------------- Data types ----------------------

@dataclass(frozen=True)
class ProfiledLayer:
    name: str
    conductance: float


@dataclass(frozen=True)
class UnprofiledEntry:
    kind: str  # e.g., "Layer", "Channel"
    name: str


@dataclass
class LayerState:
    name: str
    base_conductance: float
    current_conductance: float
    n_sublayers: int
    neglected: bool
    file_order: int  # for stable ordering / tie-break


@dataclass(frozen=True)
class MultiLineLayerProblem:
    header: str
    start_line: int
    entries: List[Tuple[int | None, float | None, float | None]]  # (Cells, Height, Conductance)


# ---------------------- Regexes ----------------------

_CONDUCTANCE_RE = re.compile(r"Conductance:\s*([+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?)")
_UNPROFILED_RE = re.compile(r"^\(!not profiled\)\s*(\w+):\s*(\S.*\S|\S)$")

# For multi-line scan: parse Cells / Height / Conductance on a "Cells:" line
_CELLS_LINE_RE = re.compile(
    r"Cells:\s*([+-]?\d+)\b.*?"
    r"Height:\s*([+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?)\b.*?"
    r"Conductance:\s*([+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?)",
    re.IGNORECASE,
)


# ---------------------- File helpers ----------------------

def _iter_lines(text: str) -> Iterator[tuple[int, str]]:
    for idx, raw in enumerate(text.splitlines(), start=1):
        yield idx, raw.rstrip("\n")


def read_file_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")

def _is_header_line(s: str) -> bool:
    """Generic header detector: non-empty, not a Cells: line, not an unprofiled marker."""
    ss = s.strip()
    if not ss:
        return False
    if ss.startswith("Cells:"):
        return False
    if _UNPROFILED_RE.match(ss):
        return False
    return True

# ---------------------- Validation scan ----------------------

def scan_multiline_layer_blocks(text: str) -> List[MultiLineLayerProblem]:
    """
    Find any header followed by >=2 'Cells:' lines (each with Conductance).
    These are reported as not acceptable.
    """
    problems: List[MultiLineLayerProblem] = []
    lines = list(_iter_lines(text))
    n = len(lines)
    i = 0

    while i < n:
        ln_no, content = lines[i]
        if _is_header_line(content):
            header = content.strip()
            j = i + 1
            entries: List[Tuple[int | None, float | None, float | None]] = []
            # Accumulate contiguous Cells: lines (allowing blank lines interspersed)
            while j < n:
                _, nxt = lines[j]
                st = nxt.lstrip()
                if not st:  # blank line => still part of same block; keep scanning
                    j += 1
                    continue
                if st.startswith("Cells:"):
                    m = _CELLS_LINE_RE.search(nxt)
                    if m:
                        cells = int(m.group(1))
                        height = float(m.group(2))
                        cond = float(m.group(3))
                        entries.append((cells, height, cond))
                    else:
                        # Cells line but cannot parse; still count as an entry with Nones
                        entries.append((None, None, None))
                    j += 1
                    continue
                # Next non-empty, non-Cells line => new header / section
                break

            if len(entries) >= 2:
                problems.append(MultiLineLayerProblem(header=header, start_line=ln_no, entries=entries))

            # Advance to j (start of next section)
            i = j
        else:
            i += 1

    return problems


# ---------------------- Parsing (strict for normal 'Layer_*') ----------------------

def parse_conductance_file(path: Path, exclude_headers: set[str] | None = None
                           ) -> tuple[List[ProfiledLayer], List[UnprofiledEntry]]:
    """
    Parse 'Layer_*' headers with a single 'Conductance:' value beneath each.
    Unprofiled entries like '(!not profiled) Layer: CONN_TO_PCB' are collected too.
    Any headers present in exclude_headers are ignored (e.g., detected multi-line blocks).
    """
    text = read_file_text(path)
    lines = [ln for _, ln in _iter_lines(text)]
    n = len(lines)
    i = 0

    profiled: List[ProfiledLayer] = []
    unprofiled: List[UnprofiledEntry] = []

    exclude_headers = exclude_headers or set()

    while i < n:
        line = lines[i].strip()

        m_un = _UNPROFILED_RE.match(line)
        if m_un:
            kind, name = m_un.group(1).strip(), m_un.group(2).strip()
            unprofiled.append(UnprofiledEntry(kind=kind, name=name))
            i += 1
            continue

        if _is_header_line(line):
            header = line
            if header in exclude_headers:
                # Skip the entire block for this header
                i += 1
                while i < n and not (_is_header_line(lines[i]) or _UNPROFILED_RE.match(lines[i].strip())):
                    i += 1
                continue

            j = i + 1
            conductance_value: float | None = None
            while j < n:
                probe = lines[j]
                if _is_header_line(probe) or _UNPROFILED_RE.match(probe.strip()):
                    break
                m_c = _CONDUCTANCE_RE.search(probe)
                if m_c:
                    conductance_value = float(m_c.group(1))
                    # Ensure there's not another 'Conductance:' before the next header
                    k = j + 1
                    multiple = False
                    while k < n:
                        nxt = lines[k].strip()
                        if _is_header_line(nxt) or _UNPROFILED_RE.match(nxt):
                            break
                        if _CONDUCTANCE_RE.search(nxt):
                            multiple = True
                            break
                        k += 1
                    if multiple:
                        exclude_headers.add(header)
                        conductance_value = None
                    break
                j += 1

            if conductance_value is None:
                # Either no conductance or malformed; skip this header block
                i = max(j, i + 1)
                continue

            profiled.append(ProfiledLayer(name=header, conductance=conductance_value))
            i = max(j + 1, i + 1)
            continue

        i += 1

    return profiled, unprofiled


# ---------------------- Math helpers ----------------------

def mark_neglected(states: List[LayerState]) -> float:
    """Mark layers whose base_conductance > 3 * initial_avg as neglected. Return initial_avg."""
    if not states:
        return 0.0
    initial_avg = sum(s.base_conductance for s in states) / len(states)
    for s in states:
        if s.base_conductance > 3.0 * initial_avg:
            s.neglected = True
    return initial_avg


def weighted_average(states: List[LayerState]) -> float | None:
    """Multiplicity-weighted average over NON-neglected layers."""
    total = 0.0
    weight = 0
    for s in states:
        if s.neglected:
            continue
        total += s.current_conductance * s.n_sublayers
        weight += s.n_sublayers
    if weight == 0:
        return None
    return total / weight


def var_s_excel_equivalent(states: List[LayerState]) -> float | None:
    """
    Excel VAR.S equivalent (sample variance) over NON-neglected layers,
    treating each layer as repeated n_sublayers times.

    VAR.S = sum((x_i - mean)^2) / (N - 1), N = total multiplicity.
    Returns None if no eligible layers; returns float('nan') if N < 2 (Excel -> #DIV/0!).
    """
    mu = weighted_average(states)
    if mu is None:
        return None
    N = sum(s.n_sublayers for s in states if not s.neglected)
    if N < 2:
        return float('nan')

    ss = 0.0
    for s in states:
        if s.neglected:
            continue
        d = s.current_conductance - mu
        ss += s.n_sublayers * d * d

    return ss / (N - 1)


def subdivide_one(s: LayerState) -> None:
    """Perform exactly one subdivision on this layer."""
    n = s.n_sublayers
    s.current_conductance *= (n + 1) / n
    s.n_sublayers = n + 1


# ---------------------- Core algorithm ----------------------

def run_phases_collect_variance(
    states: List[LayerState],
    max_total_divisions: int,
    verbose: bool,
) -> List[float]:
    """Run phases until no candidates or division budget exhausted.
    Returns a list of VAR.S values AFTER each subdivision (iteration), in order.
    """
    variances_after_each_iter: List[float] = []
    total_divisions = 0
    phase = 0

    while total_divisions < max_total_divisions:
        avg = weighted_average(states)
        if avg is None:
            if verbose:
                print("[phase ?] No eligible layers to average (all neglected). Stopping.")
            break

        # Snapshot candidates for THIS phase
        candidates = [s for s in states if (not s.neglected) and (s.current_conductance < avg)]
        if not candidates:
            if verbose:
                print(f"[phase {phase+1}] average={avg:.6g}, no candidates below average. Stopping.")
            break

        candidates.sort(key=lambda s: (s.current_conductance, s.file_order, s.name))

        phase += 1
        if verbose:
            names = ", ".join(s.name for s in candidates)
            print(f"[phase {phase}] average={avg:.6g}, {len(candidates)} candidate(s): {names}")

        for s in candidates:
            if total_divisions >= max_total_divisions:
                break
            subdivide_one(s)
            total_divisions += 1

            # Record VAR.S AFTER this subdivision.
            var = var_s_excel_equivalent(states)
            if var is not None:
                variances_after_each_iter.append(var)
            if verbose:
                var_str = f"{var:.6g}" if isinstance(var, float) else "nan"
                print(f"  - subdivided {s.name}: sublayers={s.n_sublayers}, "
                      f"new_conductance={s.current_conductance:.6g}; "
                      f"VAR.S(now)={var_str}")

    return variances_after_each_iter


# ---------------------- CLI ----------------------

def main(argv: Iterable[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Validate conductance file, subdivide low-conductance layers with a global cap, and report Excel VAR.S after each iteration."
    )
    parser.add_argument("input", type=Path, help="Path to conductance_layer.txt")
    parser.add_argument(
        "--i-max",
        type=int,
        required=True,
        help="Maximum TOTAL number of subdivisions across all phases.",
    )
    parser.add_argument(
        "--sort",
        choices=["file", "name", "conductance"],
        default="file",
        help="How to sort final output table (default: file order).",
    )
    parser.add_argument("--verbose", action="store_true", help="Print per-phase and per-division logs.")
    args = parser.parse_args(argv)

    # --- Validation scan for multi-line blocks ---
    raw_text = read_file_text(args.input)
    problems = scan_multiline_layer_blocks(raw_text)
    if problems:
        print("⚠️  Detected NOT-ACCEPTABLE multi-line layer blocks (excluded from analysis):")
        for p in problems:
            print(f"  - Header: {p.header}  (starts at line {p.start_line})")
            for idx, (cells, height, cond) in enumerate(p.entries, start=1):
                c_str = "?" if cells is None else str(cells)
                h_str = "?" if height is None else f"{height:g}"
                g_str = "?" if cond is None else f"{cond:g}"
                print(f"      line {idx:>2d}: Cells={c_str}, Height={h_str}, Conductance={g_str}")
        print()

    # Build an exclude set of offending headers (so they won't be parsed/used)
    exclude_headers = {p.header for p in problems}

    # --- Parse standard Layer_* blocks & unprofiled entries ---
    try:
        profiled, unprofiled = parse_conductance_file(args.input, exclude_headers=exclude_headers)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

    # Build initial state in file order
    states: List[LayerState] = []
    for idx, p in enumerate(profiled):
        states.append(
            LayerState(
                name=p.name,
                base_conductance=p.conductance,
                current_conductance=p.conductance,
                n_sublayers=1,
                neglected=False,
                file_order=idx,
            )
        )

    # If everything got excluded, warn and stop gracefully after printing what we found.
    if not states:
        print("No acceptable 'Layer_*' entries found for analysis after excluding problematic blocks.")
        # Still show unprofiled entries and exit 0 (validation succeeded, nothing to analyze)
        if unprofiled:
            print("\nUnprofiled entries from file:")
            for up in unprofiled:
                print(f"  {up.kind}: {up.name}")
        return 0

    initial_avg = mark_neglected(states)
    if args.verbose:
        neglected_names = [s.name for s in states if s.neglected]
        print(f"Initial average over ALL profiled layers = {initial_avg:.6g}")
        print("Neglected: " + (", ".join(neglected_names) if neglected_names else "(none)"))
    
    initial_var = var_s_excel_equivalent(states)

    variances = run_phases_collect_variance(
        states, max_total_divisions=args.i_max, verbose=args.verbose
    )

    # Prepare output
    rows = list(states)
    if args.sort == "name":
        rows.sort(key=lambda r: r.name)
    elif args.sort == "conductance":
        rows.sort(key=lambda r: r.current_conductance)
    else:  # file
        rows.sort(key=lambda r: r.file_order)

    print("\nProfiled layers:")
    print(f"{'Name':<22} {'Sublayers':>9} {'Conductance':>14}")
    print("-" * 70)
    for s in rows:
        print(
            f"{s.name:<22} {s.n_sublayers:>9d} {s.current_conductance:>14.6g}"
        )

    # Per-iteration VAR.S report with percentage vs Initial
    print("\nVariance after each iteration:")
    if initial_var is None:
        print("  (cannot compute initial variance – no eligible layers)")
    else:
        baseline = initial_var

        # Iteration 0: initial (no subdivision)
        if isinstance(initial_var, float):
            v0_str = f"{initial_var:.6g}"
        else:
            v0_str = "nan"
        print(f"  Iteration   0: {v0_str} (100.00% of Initial)")

        # Iterations 1..N: after each subdivision
        if variances:
            for i, v in enumerate(variances, start=1):
                if isinstance(v, float) and isinstance(baseline, float) and baseline > 0.0:
                    pct = (v / baseline) * 100.0
                    pct_str = f"{pct:.2f}% of Initial"
                else:
                    pct_str = "n/a (baseline=0 or invalid)"

                v_str = f"{v:.6g}" if isinstance(v, float) else "nan"
                print(f"  Iteration {i:>3d}: {v_str} ({pct_str})")
        else:
            print("  (no iterations performed)")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
