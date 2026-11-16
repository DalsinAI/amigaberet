Beret for GFOS
==============

A port of Beret, Nigel Kilmer's 2011 puzzle platform game (from the git
history kept at https://github.com/Tookmund/beret, commit
4e2f21bf4eeabfd137b2b846756a368b42456675, version 1.2.1), for GFOS:
AmigaOS 3.2 with GFOpenUp, on a 68040 or 68060 with an FPU, on GFOpenUp's
SDL 2.

Beret has telekinesis: hold the mouse button to pick up crates, enemies
and anything else he can see, and throw, stack and smash your way through
hundreds of rooms, collecting the medallions. It has a room editor too.

Status: built in Cradle's os32-gcc16-open6 stove (Beret 851,180 bytes, SDL2_ttf and FreeType linked in); not yet run in a lab.


What you need
-------------

- GFOS: AmigaOS 3.2 with GFOpenUp 0.8.26 or later, which installs
  LIBS:GFOpenGPU/SDL2.module, SDL2_image.module and SDL2_mixer.module.
- A 68040 or 68060 with an FPU (not an LC040 or EC040), an RTG screen
  with room for a 780 x 600 window, about 32 MB of free memory and 56 MB
  of disk, and a mouse.
- Nothing else: the pictures, rooms, sounds, music and font are in the
  drawer.


How to play
-----------

Install it one of three ways:

- GFOpenUp: `GFOpenUpTool INSTALL Parts/Beret` puts it in SYS:Games/Beret.
- Run Install-Beret from the archive (it offers Work:Games).
- Or copy the Beret drawer where you want it.

Then double-click Beret. It opens in a window on the Workbench screen;
Alt + Return asks for a screen of its own. The cursor keys (or W A S D)
move and jump, the left mouse button held down is telekinesis, P pauses,
Esc is the menu; the drawer's Controls file has them all. Options, saves
and the rooms you make in the editor are kept in the drawer's User drawer,
which Install-Beret and GFOpenUp keep when they update the game.


Building
--------

    sh package/build-amiga.sh        (STOVE=/path/to/os32-gcc16-open6)

or, in Cradle, stove_build with the os32-gcc16-open6 stove and target
package. It fetches upstream at the commit package/beret.lock names,
applies amiga/patches, builds with amiga/Makefile.amigaos and packs
Beret-VERSION-68040.lha and the GFOpenUp part Parts/Beret.

Host tests: `BERET=<upstream checkout> sh tests/host/test_all.sh` applies
the patch; checks that every fread and fwrite in the game reads chars or
whole 4-byte words (ints, and Thing and Particle, which are nothing else);
writes and reads ints and structs through amiga/beret_le.c built as on a
big-endian CPU; and, where the host has SDL 2, builds the game with the
Amiga's paths and SDL 2 calls and plays it on SDL's dummy drivers: the
title, a new game, into the first level, walking and jumping, and out by
the window's close, with its save in PROGDIR:User/saves.


What was changed
----------------

- amiga/patches/0001-amigaos-sdl2-drawer-paths.patch (game.c): on
  __amigaos__, the player's files in PROGDIR:User/ (made at start, with
  its rooms and saves drawers); a window at start rather than a screen;
  window focus from SDL 2's window events; amiga/beret_amiga.h included.
- amiga/beret_amiga.h, amiga/beret_sdl.c: Beret was written for SDL 1.2.
  Its SDL_SetVideoMode, SDL_Flip, SDL_DisplayFormatAlpha, SDL_WM_ and
  SDL_WarpMouse calls become SDL 2 ones. The game draws into a 32-bit
  XRGB surface (it writes some pixels itself, as 0x00RRGGBB words); each
  frame goes to the window by one blit, or straight into it when the
  window's surface has that format.
- amiga/beret_le.c: the game reads and writes its rooms, maps and saves as
  ints and structs of ints and floats, and its own files were made on a
  little-endian PC; on the 68k every 4-byte word is swapped on the way in
  and out, so the shipped files read right and saves are the same as a
  PC's.
- amiga/amiga_start.c: 512 KB of stack, the $VER string.
- amiga/: the Makefile, the GlowIcons (our own art), the Installer script
  and its spec. package/: the build, the release archive and the GFOpenUp
  part.

The AmiCradle Team.
