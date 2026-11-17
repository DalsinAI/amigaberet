/*
 * Beret on GFOS: little-endian fread/fwrite (see beret_amiga.h).
 * The swap is done on a big-endian CPU, or when BERET_LE_SWAP is defined
 * (the host tests build it that way on a PC).
 * LGPL-3.0-or-later, like Beret. Copyright (c) 2026 AmiCradle Team.
 */
#include <stdio.h>
#include <string.h>
#include "SDL.h"
#include "thing.h"

/* fread/fwrite of Thing and Particle swap them word by word: every field
 * must be a 4-byte int or float, with no padding, as on the PC that made
 * the files (46 and 6 fields). */
_Static_assert(sizeof(int) == 4 && sizeof(float) == 4, "4-byte int and float");
_Static_assert(sizeof(Thing) == 46 * 4, "Thing is 46 words");
_Static_assert(sizeof(Particle) == 6 * 4, "Particle is 6 words");

size_t beret_fread(void *ptr, size_t size, size_t n, FILE *f);
size_t beret_fwrite(const void *ptr, size_t size, size_t n, FILE *f);

#if defined(BERET_LE_SWAP) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
static void swap_words(unsigned char *p, size_t words)
{
    while (words--) {
        unsigned char t = p[0]; p[0] = p[3]; p[3] = t;
        t = p[1]; p[1] = p[2]; p[2] = t;
        p += 4;
    }
}

size_t beret_fread(void *ptr, size_t size, size_t n, FILE *f)
{
    size_t got = fread(ptr, size, n, f);
    if (size % 4 == 0) swap_words(ptr, got * size / 4);
    return got;
}

size_t beret_fwrite(const void *ptr, size_t size, size_t n, FILE *f)
{
    unsigned char buf[1024];
    const unsigned char *p = ptr;
    size_t total, done = 0;

    if (size % 4 != 0 || size == 0) return fwrite(ptr, size, n, f);
    total = size * n;
    while (done < total) {
        size_t chunk = total - done > sizeof buf ? sizeof buf : total - done;
        memcpy(buf, p + done, chunk);
        swap_words(buf, chunk / 4);
        if (fwrite(buf, 1, chunk, f) != chunk) return done / size;
        done += chunk;
    }
    return n;
}
#else
size_t beret_fread(void *ptr, size_t size, size_t n, FILE *f) { return fread(ptr, size, n, f); }
size_t beret_fwrite(const void *ptr, size_t size, size_t n, FILE *f) { return fwrite(ptr, size, n, f); }
#endif
