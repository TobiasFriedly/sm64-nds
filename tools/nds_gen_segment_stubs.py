#!/usr/bin/env python3
"""
Generate NDS segment stub definitions.

For each segment declared in segment_symbols.h, generates:
- _<name>SegmentRomStart: either a NitroFS file path (for data overlays)
  or an empty string (for segments resident in the main binary)
- _<name>SegmentRomEnd: always an empty string (unused on NDS)

The NDS load functions interpret srcStart as:
- Non-empty string: NitroFS file path to load from
- Empty string: data is resident in main binary (set segment base to 0x02000000)
"""

import sys
import re

# Segments that stay in the main binary (not loaded from NitroFS).
# These get empty path definitions so load_segment sets base to 0x02000000.
RESIDENT_SEGMENTS = {
    # Code segments
    'entry', 'engine', 'goddard',
    # Script/behavior segments (small, contain function pointers)
    'behavior', 'scripts',
}

# Geo segments are also resident (contain function pointers to main binary code)
RESIDENT_GEO_SEGMENTS = True


def parse_level_defines(level_defines_path):
    """Extract level folder names from level_defines.h."""
    levels = []
    with open(level_defines_path, 'r') as f:
        for line in f:
            m = re.match(r'\s*DEFINE_LEVEL\s*\([^,]+,[^,]+,[^,]+,\s*(\w+)\s*,', line)
            if m:
                levels.append(m.group(1))
    return levels


def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <level_defines.h> <output.c>", file=sys.stderr)
        sys.exit(1)

    level_defines_path = sys.argv[1]
    output_path = sys.argv[2]

    levels = parse_level_defines(level_defines_path)

    # Collect all segment definitions: (symbol_name, nitrofs_path_or_empty)
    segments = []

    # Actor groups: mio0 (data overlays) and geo (resident)
    for name in ['common0', 'common1'] + [f'group{i}' for i in range(18)]:
        # mio0 data: externalized to NitroFS
        segments.append((f'{name}_mio0', f'nitro:/data/actors/{name}.bin'))
        # geo data: resident in main binary (contains function pointers)
        if RESIDENT_GEO_SEGMENTS:
            segments.append((f'{name}_geo', ''))
        else:
            segments.append((f'{name}_geo', f'nitro:/data/actors/{name}_geo.bin'))

    # Internal/resident segments
    for name in ['entry', 'engine', 'behavior', 'scripts', 'goddard']:
        segments.append((name, ''))

    # Segment 2: HUD/fonts - loaded from NitroFS. segmented_to_virtual uses
    # a range check to distinguish segment 2 data refs from binary pointers.
    segments.append(('segment2_mio0', 'nitro:/data/bin/segment2.bin'))

    # Level segments: script+geo are resident, leveldata is externalized
    # Track already-defined segment names to avoid duplicates
    defined_names = set(name for name, _ in segments)

    # Special levels + regular levels
    all_levels = ['menu', 'intro', 'ending'] + levels
    for name in all_levels:
        if name not in defined_names:
            segments.append((name, ''))  # script+geo resident
            defined_names.add(name)
        seg7_name = f'{name}_segment_7'
        if seg7_name not in defined_names:
            segments.append((seg7_name, f'nitro:/data/levels/{name}/leveldata.bin'))
            defined_names.add(seg7_name)

    # Texture bins: externalized to NitroFS
    texture_bins = [
        'fire_mio0', 'spooky_mio0', 'generic_mio0', 'water_mio0',
        'sky_mio0', 'snow_mio0', 'cave_mio0', 'machine_mio0',
        'mountain_mio0', 'grass_mio0', 'outside_mio0', 'inside_mio0',
    ]
    for name in texture_bins:
        base = name.replace('_mio0', '')
        segments.append((name, f'nitro:/data/bin/{base}.bin'))

    # Effect and title screen: externalized
    segments.append(('effect_mio0', 'nitro:/data/bin/effect.bin'))
    segments.append(('title_screen_bg_mio0', 'nitro:/data/bin/title_screen_bg.bin'))

    # Debug level select
    segments.append(('debug_level_select_mio0', 'nitro:/data/bin/debug_level_select.bin'))

    # Skyboxes: externalized to NitroFS
    skyboxes = [
        'water', 'ccm', 'clouds', 'bitfs', 'wdw', 'cloud_floor',
        'ssl', 'bbh', 'bidw', 'bits',
    ]
    for name in skyboxes:
        segments.append((f'{name}_skybox_mio0', f'nitro:/data/bin/{name}_skybox.bin'))

    # Generate C file
    with open(output_path, 'w') as f:
        f.write("/**\n")
        f.write(" * Auto-generated NDS segment stubs.\n")
        f.write(" * Non-empty paths: segment data loaded from NitroFS at runtime.\n")
        f.write(" * Empty paths: segment data is resident in main binary (0x02xxxxxx).\n")
        f.write(" */\n\n")
        f.write('#include <PR/ultratypes.h>\n\n')

        resident_count = 0
        nitrofs_count = 0

        for name, path in segments:
            if path:
                f.write(f'u8 _{name}SegmentRomStart[] = "{path}";\n')
                nitrofs_count += 1
            else:
                f.write(f'u8 _{name}SegmentRomStart[] = "";\n')
                resident_count += 1
            f.write(f'u8 _{name}SegmentRomEnd[] = "";\n')

        # Extra symbols referenced in segment_symbols.h
        f.write('\n// Extra segment symbols\n')
        f.write('u8 _goddardSegmentStart[] = "";\n')

        f.write(f'\n// Total: {len(segments)} segments '
                f'({nitrofs_count} NitroFS, {resident_count} resident)\n')

    print(f"Generated {output_path}: {nitrofs_count} NitroFS + {resident_count} resident segments")


if __name__ == '__main__':
    main()
