/*
 * Host test for amiga/beret_le.c, built with BERET_LE_SWAP so a PC does
 * what a 68k does: ints and words of structs go to disk little-endian,
 * whatever the CPU, and chars as they are.
 * LGPL-3.0-or-later, like Beret. Copyright (c) 2026 AmiCradle Team.
 */
#include <stdio.h>
#include <string.h>

size_t beret_fread(void *ptr, size_t size, size_t n, FILE *f);
size_t beret_fwrite(const void *ptr, size_t size, size_t n, FILE *f);

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); fails++; } } while (0)

int main(void)
{
    /* an int as a 68k holds it is the PC's bytes swapped; BERET_LE_SWAP
     * makes the host stand in for the 68k */
    unsigned int words[700], back[700];
    unsigned char raw[2800], c = 'x', c2 = 0;
    FILE *f = tmpfile();
    size_t i;

    for (i = 0; i < 700; i++) words[i] = 0x01020304u * (unsigned)(i + 1);
    CHECK(beret_fwrite(&c, sizeof(char), 1, f) == 1);
    CHECK(beret_fwrite(words, sizeof(int), 700, f) == 700);   /* more than its 1 KB buffer */
    CHECK(beret_fwrite(words, 24, 3, f) == 3);                /* a struct of 6 words */
    rewind(f);
    CHECK(fread(&c2, 1, 1, f) == 1 && c2 == 'x');
    CHECK(fread(raw, 1, 2800, f) == 2800);
    for (i = 0; i < 700; i++) {
        unsigned int w = words[i];
        unsigned char *p = raw + 4 * i;
        CHECK(p[0] == (w >> 24 & 0xff) && p[3] == (w & 0xff));  /* swapped on disk */
    }
    CHECK(memcmp(words, raw, 4) != 0);
    words[0] = 0x11223344u;
    rewind(f);
    CHECK(beret_fread(&c2, sizeof(char), 1, f) == 1 && c2 == 'x');
    CHECK(beret_fread(back, sizeof(int), 700, f) == 700);
    words[0] = 0x01020304u;
    CHECK(memcmp(back, words, sizeof back) == 0);
    CHECK(beret_fread(back, 24, 3, f) == 3 && memcmp(back, words, 72) == 0);
    CHECK(beret_fread(back, sizeof(int), 1, f) == 0);           /* end of file */
    fclose(f);
    printf("le: chars as they are, %d ints and 3 structs swapped and back%s\n", 700, fails ? "" : ", ok");
    return fails != 0;
}
