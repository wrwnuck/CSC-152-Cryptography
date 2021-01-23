#include <string.h>
#include <stdio.h>
#include <stdint.h>

uint64_t rotl64(uint64_t x, int n) {
    return (x<<n) | (x>>(64-n));
}

uint64_t load64be(void *p) {
    // Replace with your tested solution
    uint8_t *bytes = (uint8_t *)p;
    uint64_t a = bytes[0];
    uint64_t b = bytes[1];
    uint64_t c = bytes[2];
    uint64_t d = bytes[3];
    uint64_t e = bytes[4];
    uint64_t f = bytes[5];
    uint64_t g = bytes[6];
    uint64_t h = bytes[7];
    return (a<<56) | (b<<48) | (c<<40) | (d<<32) | (e<<24) | (f<<16) | (g<<8)| h;
}

void store64be(void *p, uint64_t x) {
    // Replace with your tested solution
    uint8_t *bytes = (uint8_t *)p;
    bytes[0] = (uint8_t)((x >> 56) & 0xFF);
    bytes[1] = (uint8_t)((x >> 48) & 0x00FF);
    bytes[2] = (uint8_t)((x >> 40) & 0x0000FF);
    bytes[3] = (uint8_t)((x >> 32) & 0x000000FF);
    bytes[4] = (uint8_t)((x >> 24) & 0x00000000FF);
    bytes[5] = (uint8_t)((x >> 16) & 0x0000000000FF);
    bytes[6] = (uint8_t)((x >> 8) & 0x000000000000FF);
    bytes[7] = (uint8_t) x & 0x00000000000000FF;
}

void mix(uint64_t v[4]) {
    uint64_t v0 = v[0];
    uint64_t v1 = v[1];
    uint64_t v2 = v[2];
    uint64_t v3 = v[3];
    
    // Implement dataflow diagram here
    v0 = v1 + v0;
    v1 = rotl64(v1, 13);
    v2 = v3 + v2;
    v3 = rotl64(v3, 16);
    
    v1 = v1 ^ v0;
    v0 = rotl64(v0,32);
    v3 = v2 ^ v3;
    
    v0 = v0 + v3;
    v3 = rotl64(v3,21);
    v2 = v2 + v1;
    v1 = rotl64(v1,17);
    
    v1 = v2 ^ v1;
    v2 = rotl64(v2,32);
    v3 = v3 ^ v0;
    
    
    v[0] = v0; 
    v[1] = v1; 
    v[2] = v2; 
    v[3] = v3; 
}

void checksum(void *in, int nbytes, uint64_t *out) {
    uint64_t data;                       // Each read goes here
    uint8_t buf[8] = {0,0,0,0,0,0,0,0};  // Final bytes copied here, init to 0's
    uint64_t v[4] =                      // Initialize v to constants
        {UINT64_C(0xb7e151628aed2a6b),UINT64_C(0x9e3779b97f4a7c15),0,0};
    uint64_t *next = (uint64_t *)in;     // Want uint64_t's so copy address
    while (nbytes >= 8) {        // While I can read a ful 8 bytes
        data = load64be(next);   // Read 8 bytes big-endian
        v[3] ^= data;            // XOR data into v
        mix(v);                  // mix v twice
        mix(v);
        v[1] ^= data;            // XOR data into v again
        next += 1;               // Increment next pointer 8 bytes
        nbytes -= 8;             // We have 8 few bytes left to process
    }
    memcpy(buf,next,nbytes);     // Copy remaining bytes to process, 0-7
    buf[nbytes] = 0b10000000;    // Make rest of buf 1 bit followed by all 0 bits
    data = load64be(buf);        // Read remaining bytes (and 100...0 bits)
    v[3] ^= data;                // XOR data into v
    mix(v);                      // mix v twice
    mix(v);
    v[1] ^= data;                // XOR data into v again
    mix(v);                      // Final mixing, four times
    mix(v);
    mix(v);
    mix(v);
    store64be(out, v[0] ^ v[1] ^ v[2] ^ v[3]);  // XOR v parts, write big-endian
}

/*
int main() {
    // Write your tests here, but delete main before submission
    uint64_t v[4] = {0,2,3,0};
    mix(v);
    printf("v0 = %lx\n",v[0]);
    printf("v1 = %lx\n",v[1]);
    printf("v2 = %lx\n",v[2]);
    printf("v3 = %lx\n",v[3]);
    
    
}
*/
