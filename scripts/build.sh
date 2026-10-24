#!/usr/bin/env bash
# Full build: hardware (Vivado) first, then software (Vitis) on top of the exported XSA.
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

# Keep Vivado's log and journal out of the repository root
mkdir -p build
vivado -mode batch -notrace \
    -log build/vivado.log -journal build/vivado.jou \
    -source hw/build.tcl

vitis -s sw/build.py
