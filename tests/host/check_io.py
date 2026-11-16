#!/usr/bin/env python3
"""Every fread/fwrite in the patched game.c reads chars, or whole 4-byte
words (int, Thing, Particle), so amiga/beret_le.c can keep each word
little-endian on disk. Usage: check_io.py <beret>/game.c"""
import re, sys
src = open(sys.argv[1], encoding="latin-1").read()
calls = []
for m in re.finditer(r"\bf(read|write)\(", src):
    depth, i = 1, m.end()
    while depth:
        depth += {"(": 1, ")": -1}.get(src[i], 0)
        i += 1
    calls.append((m.group(1), src[m.end():i - 1]))
sizes = {}
bad = []
for kind, args in calls:
    m = re.search(r"sizeof\((\w+)\)", args)
    if not m:
        bad.append((kind, args))
        continue
    sizes[m.group(1)] = sizes.get(m.group(1), 0) + 1
    if m.group(1) not in ("char", "int", "Thing", "Particle"):
        bad.append((kind, args))
print("io: %d fread/fwrite calls, element sizes %s" % (len(calls), ", ".join("%s %d" % kv for kv in sorted(sizes.items()))))
if bad or len(calls) < 100:
    print("FAIL:", bad or "too few calls found")
    sys.exit(1)
