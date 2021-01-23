#include <immintrin.h>

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
        __m128i ys = _mm_or_si128(_mm_slli_epi32(r1,9), _mm_srli_epi32(r1,23));
        __m128i zs = r2;
        r2 = _mm_xor_si128(xs, _mm_xor_si128(_mm_slli_epi32(zs, 1), _mm_slli_epi32(_mm_and_si128(ys, zs), 2)));
        r1 = _mm_xor_si128(ys, _mm_xor_si128(xs, _mm_slli_epi32(_mm_or_si128(xs, zs), 1)));
        r0 = _mm_xor_si128(zs, _mm_xor_si128(ys, _mm_slli_epi32(_mm_and_si128(xs, ys), 3)));
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



