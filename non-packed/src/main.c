#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define SERIAL_LEN 16


static int check(const char* s)
{
    if (strlen(s) != SERIAL_LEN)
        return 0;

    /* Only printable ASCII */
    for (int i = 0; i < SERIAL_LEN; ++i) {
        unsigned char c = (unsigned char)s[i];
        if (c < 0x21 || c > 0x7E)
            return 0;
    }

    
    uint32_t acc = 0xC0FFEE13u;
    uint32_t st = 0x1337u;

    for (int i = 0; i < SERIAL_LEN; ++i) {
        uint32_t x = (uint8_t)s[i];

        switch (i & 3) {
        case 0: acc = (acc + x * 0x9E3779B1u) ^ st;            break;
        case 1: acc = ((acc << 7) | (acc >> 25)) - (x * 31u);  break;
        case 2: acc ^= (x + 0xDEADBEEFu) * (st | 1u);          break;
        case 3: acc = (acc * 0x85EBCA77u) + (x ^ 0xA5A5u);     break;
        }

        st = (st * 1103515245u + 12345u) ^ acc;
    }

    
    uint32_t pair = 0;
    for (int i = 0; i < SERIAL_LEN; i += 2) {
        pair += (uint32_t)(uint8_t)s[i] * (uint32_t)(uint8_t)s[i + 1];
    }

    
    const uint32_t TARGET_ACC = 0xB16B00B5u;
    const uint32_t TARGET_PAIR = 0x0000A11Eu;

    return (acc == TARGET_ACC) && (pair == TARGET_PAIR);
}


int main(void)
{
    char buf[64];

    printf("protector demo crackme \n");
    printf("enter serial (%d chars, printable ASCII): ", SERIAL_LEN);
    fflush(stdout);

    if (!fgets(buf, sizeof(buf), stdin)) {
        return 1;
    }

    /* strip trailing newline */
    size_t n = strlen(buf);
    while (n && (buf[n - 1] == '\n' || buf[n - 1] == '\r'))
        buf[--n] = 0;

    if (check(buf)) {
        printf("correct — nice work.\n");
        return 0;
    }
    else {
        printf("wrong.\n");
        return 1;
    }
}