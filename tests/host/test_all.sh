#!/bin/sh
# Host tests for the Beret port. BERET=<upstream checkout at the locked
# commit, unpatched>; the patch is applied to a copy.
# LGPL-3.0-or-later, like Beret. Copyright (c) 2026 AmiCradle Team.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/../.." && pwd)
: "${BERET:?set BERET to an upstream checkout}"
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
cp -R "$BERET" "$T/beret"
(cd "$T/beret" && for p in "$ROOT"/amiga/patches/*.patch; do patch -s -p1 < "$p"; done)
echo "patches: apply"
grep -q 'BERET_USER_PATH' "$T/beret/game.c" && grep -q 'SDL_WINDOWEVENT_FOCUS_LOST' "$T/beret/game.c"
python3 "$HERE/check_io.py" "$T/beret/game.c"
cc -DBERET_LE_SWAP -I"$T/beret" $(pkg-config --cflags sdl2 2>/dev/null || echo -I/usr/include/SDL2) \
    "$HERE/test_le.c" "$ROOT/amiga/beret_le.c" -o "$T/test_le"
"$T/test_le"
if pkg-config --exists sdl2 SDL2_image SDL2_ttf SDL2_mixer; then
    # the game itself, with the Amiga's paths (PROGDIR:Data/, PROGDIR:User/
    # are plain directory names on the host) and its SDL 2 calls, played
    # by tests/host/play_driver.c on SDL's dummy drivers
    cd "$T"
    for c in beret/game.c beret/thing.c beret/physics.c "$ROOT/amiga/beret_sdl.c" "$ROOT/amiga/beret_le.c"; do
        cc -O1 -w -D__amigaos__ -Dmain=beret_game_main -DRESOURCE_PATH='"PROGDIR:Data/"' \
            -I"$ROOT/amiga" -I"$T/beret" $(pkg-config --cflags sdl2) -c "$c" -o "$(basename "$c" .c).o"
    done
    cc $(pkg-config --cflags sdl2) "$HERE/play_driver.c" game.o thing.o physics.o beret_sdl.o beret_le.o \
        -o play $(pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer) -lm
    mkdir "PROGDIR:Data"
    for d in images music sfx rooms AveriaSans-Regular.ttf; do ln -s "$T/beret/$d" "PROGDIR:Data/$d"; done
    SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./play 2>&1 | grep -v iCCP
    test -d "PROGDIR:User/rooms" && test -s "PROGDIR:User/saves/game1.gm"
    echo "play: title, new game AMI, in a level; PROGDIR:User/saves/game1.gm written"
else
    echo "play: SKIP (no SDL 2 development files on this host)"
fi
echo PASS
