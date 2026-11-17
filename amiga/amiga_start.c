/* The Amiga port's start: the stack and the version string. The game
 * makes PROGDIR:User (options, saves, rooms of its own) itself
 * (amiga/patches).
 * LGPL-3.0-or-later, like Beret. Copyright (c) 2026 AmiCradle Team. */

/* A Shell gives 4 KB of stack; SDL 2 and the game's big local arrays want
 * more. libnix's startup sets this much up. */
unsigned long __stack = 512 * 1024;
static const char version[] __attribute__((used)) =
    "$VER: Beret 1.2.1 (16.11.2026)";
