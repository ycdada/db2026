#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'USAGE'
Usage:
  scripts/profile_flamegraph.sh [options] -- <command> [args...]
  scripts/profile_flamegraph.sh [options] --pid <pid> --duration <seconds>

Options:
  -o, --output-dir <dir>        Output directory. Default: build/profile
  -F, --freq <hz>              Sampling frequency. Default: 99
      --script-timeout <sec>   Timeout for perf script conversion. Default: 120
      --flamegraph-dir <dir>   Directory containing stackcollapse-perf.pl and flamegraph.pl.
                               Defaults to FLAMEGRAPH_DIR, then ~/WSL2/FlameGraph.

Examples:
  cmake -S . -B build-profile -DCMAKE_BUILD_TYPE=RelWithDebInfo
  cmake --build build-profile -j
  scripts/profile_flamegraph.sh --pid $(pidof rmdb) --duration 60
  scripts/profile_flamegraph.sh --pid $(pidof rmdb) --duration 60 --flamegraph-dir ~/WSL2/FlameGraph
  scripts/profile_flamegraph.sh -- build-profile/bin/unit_test
USAGE
}

OUT_DIR="build/profile"
FREQ="99"
SCRIPT_TIMEOUT="120"
PID=""
DURATION=""
FLAMEGRAPH_DIR="${FLAMEGRAPH_DIR:-}"
COMMAND=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        -o|--output-dir)
            OUT_DIR="$2"
            shift 2
            ;;
        -F|--freq)
            FREQ="$2"
            shift 2
            ;;
        --pid)
            PID="$2"
            shift 2
            ;;
        --duration)
            DURATION="$2"
            shift 2
            ;;
        --flamegraph-dir)
            FLAMEGRAPH_DIR="$2"
            shift 2
            ;;
        --script-timeout)
            SCRIPT_TIMEOUT="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        --)
            shift
            COMMAND=("$@")
            break
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

if ! command -v perf >/dev/null 2>&1; then
    echo "perf is required but was not found in PATH." >&2
    exit 1
fi

if [[ -n "$PID" && -z "$DURATION" ]]; then
    echo "--duration is required when --pid is used." >&2
    exit 2
fi

if [[ -z "$PID" && ${#COMMAND[@]} -eq 0 ]]; then
    echo "Provide either --pid/--duration or a command after --." >&2
    usage >&2
    exit 2
fi

mkdir -p "$OUT_DIR"
PERF_DATA="$OUT_DIR/perf.data"
PERF_SCRIPT="$OUT_DIR/perf.script"
FOLDED="$OUT_DIR/stacks.folded"
SVG="$OUT_DIR/flamegraph.svg"

if [[ -n "$PID" ]]; then
    perf record -F "$FREQ" -g --call-graph fp -p "$PID" -o "$PERF_DATA" -- sleep "$DURATION"
else
    perf record -F "$FREQ" -g --call-graph fp -o "$PERF_DATA" -- "${COMMAND[@]}"
fi

timeout "${SCRIPT_TIMEOUT}s" perf script --no-inline -i "$PERF_DATA" > "$PERF_SCRIPT"

find_flamegraph_tool() {
    local tool="$1"
    if [[ -n "$FLAMEGRAPH_DIR" && -x "$FLAMEGRAPH_DIR/$tool" ]]; then
        echo "$FLAMEGRAPH_DIR/$tool"
        return 0
    fi
    local wsl2_flamegraph="$HOME/WSL2/FlameGraph"
    if [[ -x "$wsl2_flamegraph/$tool" ]]; then
        echo "$wsl2_flamegraph/$tool"
        return 0
    fi
    if command -v "$tool" >/dev/null 2>&1; then
        command -v "$tool"
        return 0
    fi
    return 1
}

STACKCOLLAPSE="$(find_flamegraph_tool stackcollapse-perf.pl || true)"
FLAMEGRAPH="$(find_flamegraph_tool flamegraph.pl || true)"

if [[ -n "$STACKCOLLAPSE" ]]; then
    "$STACKCOLLAPSE" "$PERF_SCRIPT" > "$FOLDED"
else
    echo "stackcollapse-perf.pl was not found; wrote raw perf script to $PERF_SCRIPT." >&2
    exit 0
fi

if [[ -n "$FLAMEGRAPH" ]]; then
    "$FLAMEGRAPH" "$FOLDED" > "$SVG"
    echo "Wrote $SVG"
else
    echo "flamegraph.pl was not found; wrote folded stacks to $FOLDED." >&2
fi
