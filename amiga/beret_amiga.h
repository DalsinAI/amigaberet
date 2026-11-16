/*
 * Beret on GFOS: what the game's SDL 1.2 calls become on SDL 2, the
 * player's drawer, and little-endian save and room files.
 * Included by game.c (amiga/patches) on __amigaos__ only.
 * LGPL-3.0-or-later, like Beret. Copyright (c) 2026 AmiCradle Team.
 */
#ifndef BERET_AMIGA_H
#define BERET_AMIGA_H

#include <stdio.h>
#include "SDL.h"

/* the player's files: options, saves, rooms made in the editor */
#ifndef BERET_USER_DIR
#define BERET_USER_DIR  "PROGDIR:User"
#endif
#ifndef BERET_USER_PATH
#define BERET_USER_PATH "PROGDIR:User/"
#endif

/* SDL 1.2 names */
typedef SDL_Keycode SDLKey;
typedef SDL_Keymod SDLMod;
#ifndef KMOD_META
#define KMOD_META KMOD_GUI
#endif
#ifndef SDL_FULLSCREEN
#define SDL_FULLSCREEN 0x80000000u
#endif
#undef SDL_INIT_EVERYTHING
#define SDL_INIT_EVERYTHING (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)

SDL_Surface *beret_set_video_mode(int w, int h, int bpp, Uint32 flags);
int beret_flip(SDL_Surface *screen);
SDL_Surface *beret_display_format_alpha(SDL_Surface *s);
void beret_wm_set_caption(const char *title, const char *icon);
void beret_wm_set_icon(SDL_Surface *icon, Uint8 *mask);
void beret_warp_mouse(Uint16 x, Uint16 y);

#define SDL_SetVideoMode beret_set_video_mode
#define SDL_Flip beret_flip
#define SDL_DisplayFormatAlpha beret_display_format_alpha
#define SDL_WM_SetCaption beret_wm_set_caption
#define SDL_WM_SetIcon beret_wm_set_icon
#define SDL_WarpMouse beret_warp_mouse

/* The game writes ints, floats and structs of them with fread/fwrite;
 * its own rooms, maps and level files were made on a little-endian PC.
 * Every element whose size is a multiple of 4 is kept little-endian on
 * disk, so the shipped files read right and saves match a PC's. */
size_t beret_fread(void *ptr, size_t size, size_t n, FILE *f);
size_t beret_fwrite(const void *ptr, size_t size, size_t n, FILE *f);
#define fread(p, s, n, f) beret_fread((p), (s), (n), (f))
#define fwrite(p, s, n, f) beret_fwrite((p), (s), (n), (f))

#endif
