#include <stdio.h>
#include <stdint.h>

uint32_t load32le(void *addr) {
    uint8_t *p = (uint8_t *)addr;
    uint32_t a = p[0];   // least significant byte of a is byte from p[0]
    uint32_t b = p[1];   // least significant byte of b is byte from p[1]
    uint32_t c = p[2];   // least significant byte of c is byte from p[2]
    uint32_t d = p[3];   // least significant byte of d is byte from p[3]
    return (d<<24) | (c<<16) | (b<<8) | a;  // shift each byte into place
}

void store32le(uint32_t x, void *addr) {
    uint8_t *p = (uint8_t *)addr;
    p[0] = (uint8_t)x;         // Typecast says keep least significant byte
    p[1] = (uint8_t)(x >> 8);  // Typecast says keep least significant byte
    p[2] = (uint8_t)(x >> 16); // Typecast says keep least significant byte
    p[3] = (uint8_t)(x >> 24); // Typecast says keep least significant byte

}

uint32_t rotl32(uint32_t x, int n) {
    return (x << n) | (x >> (32-n));
}

// src and dst are each addresses of 48 byte buffers. Okay if they overlap.
void perm152(void *src, void *dst) {
    uint32_t t,x,y,z,s[3][4];  // s is 48 bytes auto allocated on the stack
    // Load data little endian into s
    for (int r=0; r<3; r++)
        for (int c=0; c<4; c++)
            s[r][c] = load32le((uint32_t *)src+r*4+c);
    for (int round=0; round<24; round++) {
        for (int c=0; c<4; c++) {
            // Your code goes here
            x = rotl32(s[0][c], 24);
            y = rotl32(s[1][c], 9);
            z = s[2][c];
            s[2][c] = x ^ (z << 1) ^ ((y & z) << 2);
            s[1][c] = y ^ x ^ ((x | z) << 1);
            s[0][c] = z ^ y ^ ((x & y) << 3);
            
        }
        if (round % 4 == 0) {
            t=s[0][0]; s[0][0]=s[0][1]; s[0][1]=t;
            t=s[0][2]; s[0][2]=s[0][3]; s[0][3]=t;
            s[0][0] = s[0][0] ^ 0x9e377900 ^ (24-round);
        } else if (round % 4 == 2) {
            t=s[0][0]; s[0][0]=s[0][2]; s[0][2]=t;
            t=s[0][1]; s[0][1]=s[0][3]; s[0][3]=t;
        }
    }
    // Store data little endian from s
    for (int r=0; r<3; r++)
        for (int c=0; c<4; c++)
            store32le(s[r][c], (uint32_t *)dst+r*4+c);
}

