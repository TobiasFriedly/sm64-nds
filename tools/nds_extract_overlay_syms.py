#!/usr/bin/env python3
"""
Extract global symbol addresses from overlay ELF files and generate
a linker script fragment with PROVIDE directives.

This allows the main binary to reference symbols defined in overlays
(loaded at runtime from NitroFS) at their correct segment addresses.
"""

import subprocess
import sys
import re
import os

def extract_symbols(nm_path, elf_path):
    """Extract all global defined symbols from an ELF file using nm."""
    result = subprocess.run(
        [nm_path, '--defined-only', '-g', elf_path],
        capture_output=True, text=True
    )
    symbols = {}
    for line in result.stdout.strip().split('\n'):
        if not line:
            continue
        parts = line.split()
        if len(parts) >= 3:
            addr = int(parts[0], 16)
            sym_type = parts[1]
            name = parts[2]
            # Skip internal/local symbols
            if name.startswith('.') or name.startswith('$'):
                continue
            symbols[name] = addr
    return symbols

def main():
    if len(sys.argv) < 4:
        print(f"Usage: {sys.argv[0]} <nm_path> <output.ld> <overlay1.elf> [overlay2.elf ...]",
              file=sys.stderr)
        sys.exit(1)

    nm_path = sys.argv[1]
    output_path = sys.argv[2]
    elf_files = sys.argv[3:]

    all_symbols = {}
    for elf in elf_files:
        if not os.path.exists(elf):
            print(f"Warning: {elf} not found, skipping", file=sys.stderr)
            continue
        syms = extract_symbols(nm_path, elf)
        for name, addr in syms.items():
            if name in all_symbols and all_symbols[name] != addr:
                # Same symbol at different addresses in different overlays
                # This is expected for shared actor group data
                # Use the first definition (they should be consistent within each overlay)
                pass
            else:
                all_symbols[name] = addr

    with open(output_path, 'w') as f:
        f.write("/* Auto-generated overlay symbol definitions for NDS */\n")
        f.write("/* These provide addresses of data in segment overlays */\n\n")
        for name in sorted(all_symbols.keys()):
            addr = all_symbols[name]
            f.write(f'PROVIDE({name} = 0x{addr:08X});\n')

    print(f"Generated {output_path} with {len(all_symbols)} symbol definitions")

if __name__ == '__main__':
    main()
