/*
 * Beret on GFOS: the game's SDL 1.2 screen calls on SDL 2 (beret_amiga.h).
 * The game draws into a 32-bit XRGB surface (its putpixel writes Uint32
 * 0x00RRGGBB words); a frame goes to the window's surface by one blit,
 * or the window's own surface is used when it already has that format.
 * LGPL-3.0-or-later, like Beret. Copyright (c) 2026 AmiCradle Team.
 */
#include "SDL.h"
#include "beret_amiga.h"

static SDL_Window *win;
static SDL_Surface *shadow;
static SDL_Surface *icon_surface;
static char caption[64] = "Beret";

static SDL_Surface *screen_for(SDL_Window *w)
{
    SDL_Surface *ws = SDL_GetWindowSurface(w);
    if (!ws) return NULL;
    if (ws->format->format == SDL_PIXELFORMAT_RGB888) {
        if (shadow) { SDL_FreeSurface(shadow); shadow = NULL; }
        return ws;
    }
    if (!shadow || shadow->w != ws->w || shadow->h != ws->h) {
        if (shadow) SDL_FreeSurface(shadow);
        shadow = SDL_CreateRGBSurfaceWithFormat(0, ws->w, ws->h, 32, SDL_PIXELFORMAT_RGB888);
    }
    return shadow;
}

SDL_Surface *beret_set_video_mode(int w, int h, int bpp, Uint32 flags)
{
    (void)bpp;
    if (!win) {
        win = SDL_CreateWindow(caption, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               w, h, (flags & SDL_FULLSCREEN) ? SDL_WINDOW_FULLSCREEN : 0);
        if (!win) return NULL;
        if (icon_surface) SDL_SetWindowIcon(win, icon_surface);
    } else if (SDL_SetWindowFullscreen(win, (flags & SDL_FULLSCREEN) ? SDL_WINDOW_FULLSCREEN : 0) != 0) {
        SDL_SetWindowFullscreen(win, 0);   /* no screen mode for it: stay in the window */
    }
    return screen_for(win);
}

int beret_flip(SDL_Surface *screen)
{
    SDL_Surface *ws;
    if (!win) return -1;
    ws = SDL_GetWindowSurface(win);
    if (!ws) return -1;
    if (screen != ws && SDL_BlitSurface(screen, NULL, ws, NULL) != 0) return -1;
    return SDL_UpdateWindowSurface(win);
}

SDL_Surface *beret_display_format_alpha(SDL_Surface *s)
{
    SDL_Surface *c = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ARGB8888, 0);
    if (c) SDL_SetSurfaceBlendMode(c, SDL_BLENDMODE_BLEND);
    return c;
}

void beret_wm_set_caption(const char *title, const char *icon)
{
    (void)icon;
    SDL_strlcpy(caption, title ? title : "Beret", sizeof caption);
    if (win) SDL_SetWindowTitle(win, caption);
}

void beret_wm_set_icon(SDL_Surface *icon, Uint8 *mask)
{
    (void)mask;
    icon_surface = icon;
    if (win && icon) SDL_SetWindowIcon(win, icon);
}

void beret_warp_mouse(Uint16 x, Uint16 y)
{
    if (win) SDL_WarpMouseInWindow(win, x, y);
}
