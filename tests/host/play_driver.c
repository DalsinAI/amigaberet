/*
 * Host test: runs Beret (built with the Amiga paths and SDL 2 calls) on
 * SDL's dummy video and audio drivers and plays it with queued keys: the
 * title, the game select, a new game, some walking and jumping, then the
 * window's close. game.c is built with -Dmain=beret_game_main.
 * LGPL-3.0-or-later, like Beret. Copyright (c) 2026 AmiCradle Team.
 */
#include "SDL.h"
#include <stdio.h>

int beret_game_main(int argc, char *argv[]);
extern int ingame;       /* game.c: 2 while a level is being played */

static const struct { Uint32 at; SDL_Keycode key; int down; } script[] = {
    { 800, SDLK_SPACE, 1 }, { 850, SDLK_SPACE, 0 },     /* title */
    { 1600, SDLK_RETURN, 1 }, { 1650, SDLK_RETURN, 0 }, /* game 1: new */
    { 2000, SDLK_a, 1 }, { 2050, SDLK_a, 0 },           /* name: AMI */
    { 2100, SDLK_m, 1 }, { 2150, SDLK_m, 0 },
    { 2200, SDLK_i, 1 }, { 2250, SDLK_i, 0 },
    { 2400, SDLK_RETURN, 1 }, { 2450, SDLK_RETURN, 0 },
    { 4000, SDLK_RETURN, 1 }, { 4050, SDLK_RETURN, 0 }, /* past the story */
    { 5000, SDLK_RIGHT, 1 }, { 6000, SDLK_RIGHT, 0 },   /* walk and jump */
    { 6100, SDLK_UP, 1 }, { 6200, SDLK_UP, 0 },
    { 6300, SDLK_LEFT, 1 }, { 7000, SDLK_LEFT, 0 },
};

static Uint32 tick(Uint32 interval, void *param)
{
    static unsigned next;
    Uint32 now = SDL_GetTicks();
    (void)param;
    while (next < sizeof script / sizeof script[0] && script[next].at <= now) {
        SDL_Event e;
        SDL_zero(e);
        e.type = script[next].down ? SDL_KEYDOWN : SDL_KEYUP;
        e.key.keysym.sym = script[next].key;
        e.key.state = script[next].down ? SDL_PRESSED : SDL_RELEASED;
        SDL_PushEvent(&e);
        next++;
    }
    if (now > 9000) {
        SDL_Event q;
        SDL_zero(q);
        q.type = SDL_QUIT;
        SDL_PushEvent(&q);
        return 0;
    }
    return interval;
}

int main(int argc, char *argv[])
{
    int rc;
    SDL_Init(SDL_INIT_TIMER);
    SDL_AddTimer(50, tick, NULL);
    rc = beret_game_main(argc, argv);
    printf("beret returned %d, ingame %d\n", rc, ingame);
    return rc != 0 || ingame != 2;
}
