#include <immintrin.h>
#include <string.h>
#include <stdint.h>


// Sponge constants
#define R 16     // Rate: bytes of 'in' read per iteration
#define C 32     // Capacity: part of state that 'in' doesn't touch
#define B 48     // Block length

#define IN_SZ 40
#define OUT_SZ 39

void perm152hash256(void *in, int inbytes, void *out);
// src and dst are each addresses of 48 byte buffers. Okay if buffers overlap.
void perm152(void *src, void *dst) {
    // Copy addresses into pointers of more convenient type
    __m128i *s = (__m128i *)src;
    __m128i *d = (__m128i *)dst;
    // Load three rows, each row will be treated as 4 unit32_t data types
    __m128i r0 = _mm_loadu_si128(s+0);
    __m128i r1 = _mm_loadu_si128(s+1);
    __m128i r2 = _mm_loadu_si128(s+2);
    for (int round=0; round<24; round++) {
        __m128i xs = _mm_or_si128(_mm_slli_epi32(r0,24), _mm_srli_epi32(r0, 8)); 
        __m128i ys = _mm_or_si128(_mm_slli_epi32(r1, 9), _mm_srli_epi32(r1,23));
        __m128i zs = r2;
        r2 = _mm_xor_si128(
                _mm_xor_si128(xs, _mm_slli_epi32(zs,1)),
                _mm_slli_epi32(_mm_and_si128(ys, zs),2));
        r1 = _mm_xor_si128(
                _mm_xor_si128(ys, xs),
                _mm_slli_epi32(_mm_or_si128(xs, zs),1));
        r0 = _mm_xor_si128(
                _mm_xor_si128(zs, ys),
                _mm_slli_epi32(_mm_and_si128(xs, ys),3));
        if (round % 4 == 0)
            r0 = _mm_xor_si128(_mm_shuffle_epi32(r0,0b10110001),
                 _mm_set_epi32(0,0,0,0x9e377900 + (24-round)));
        else if (round % 4 == 2)
            r0 = _mm_shuffle_epi32(r0,0b01001110);
    }
    // Store three rows to dst 
    _mm_storeu_si128(d+0,r0);
    _mm_storeu_si128(d+1,r1);
    _mm_storeu_si128(d+2,r2);
}

// Reads inbytes from in and writes outbytes to out
void perm152sponge(void *in, int inbytes, void *out, int outbytes){
    // Write your code here
    uint8_t *p = (uint8_t *)in;
    uint8_t tail[R];
    uint8_t block[B];
    int remaining = inbytes;
    int i = 0;
    
    for(i = 0; i < B; i++){
        block[i] = 0;
    }
    for (i = 0; i < R; i++){
        tail[i] = 0;
    }
    
    while(remaining >= R){
        for (int i=0; i<R; i++)
            block[i] ^= p[i];
        perm152(block, block);
        p += R;
        remaining -= R;
    }
    
    memcpy(tail, p, remaining);

    tail[remaining] = 0x80;
    for (int i = remaining+1; i < R; i++)
    {
        tail[i] = 0;
    }
    
    tail[R-1] = tail[R-1] | 0x01;

    for (int i=0; i<R; i++)
            block[i] ^= tail[i];
            
    perm152(block, block);

    uint8_t result[500];
    for (i = 0; i < 500; i++){
        result[i] = 0;
    }
    
    memcpy(result, block, R);

    remaining = outbytes;
    
    i = 1;
    while (remaining >= R)
    {
        perm152(block, block);
        memcpy(result+(R*i), block, R);
        remaining -= R;
        i++;
    }

    perm152(block, block);
    memcpy(result+(R*i), block, remaining);
    memcpy(out, result, outbytes);

    
}

// Reads inbytes from in and writes 32 bytes to out
void perm152hash256(void *in, int inbytes, void *out) {
    perm152sponge(in, inbytes, out, 32);
}






