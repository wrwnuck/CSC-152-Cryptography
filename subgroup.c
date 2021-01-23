
// Written by: Warren Wnuck

#include <stdio.h>
#include <stdlib.h>
#include <openssl/bn.h>


#define CONFIDENCE_LEVEL 64   


static int probably_prime(BIGNUM *p, BN_CTX *ctx) {
    int i = 0;
    int found_p_minus_1 = 0;
    
    BIGNUM *x = BN_new();
    BIGNUM *tmp = BN_new();
    BIGNUM *tmpP;
    BIGNUM *pM1;
    
    tmpP = BN_dup(p);
    pM1 = BN_dup(p);
    BN_sub_word(pM1,1);
        
    BN_sub_word(tmpP, 1);
    BN_div_word(tmpP, 2);
        
    for (i = 0; i < CONFIDENCE_LEVEL; i++){
            
        BN_rand_range(x, pM1);
        BN_add_word(x, 1);
            
        BN_mod_exp(tmp, x, tmpP, p, ctx);
          
        if (!BN_is_one(tmp) && BN_cmp(tmp, pM1)){
            
            BN_free(x);
            BN_free(tmp);
            BN_free(tmpP);
            BN_free(pM1);
            return 0;
        }
        if (!BN_cmp(tmp, pM1)){
            found_p_minus_1 = 1;   
        }
            
    }
    
    BN_free(x);
    BN_free(tmp);
    BN_free(tmpP);
    BN_free(pM1);
    return found_p_minus_1;
}


static void gen_prime(BIGNUM *r, int bits, BN_CTX *ctx) {
    
    do{
        
        BN_rand(r, bits, BN_RAND_TOP_ONE, BN_RAND_BOTTOM_ODD);
        
    }while(!(probably_prime(r, ctx)));
    
}


void gen_group(BIGNUM *p, BIGNUM *q, BIGNUM *g, int pbits, int qbits) {
    
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *n = BN_new();
    
    gen_prime(q, qbits, ctx);
    
    do{
        BN_rand(n, (pbits-qbits), BN_RAND_TOP_ONE, BN_RAND_BOTTOM_ANY);
        BN_clear_bit(n,0);
        BN_mul(p, n, q, ctx);
        BN_add_word(p, 1);

    }while(!probably_prime(p, ctx));
    
   
    BIGNUM *a = BN_new();
    BN_one(a);
    
    do{
        
        BN_add_word(a, 1);
        BN_mod_exp(g, a, n, p, ctx);
        BN_add_word(p, 0);
    }while(BN_is_one(g));
    
    BN_CTX_free(ctx);
    BN_free(n);
    BN_free(a);
    
}


