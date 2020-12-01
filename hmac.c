#include "hmac.h"

#include "md5.h"
#include "stdio.h"
#include "string.h"

void hmac_md5(const unsigned char* input, unsigned int length,
              const unsigned char* key, unsigned int key_len,
              unsigned char* ref) {
    unsigned char ipad[BLOCKSIZE + 1];
    unsigned char opad[BLOCKSIZE + 1];
    unsigned char kplus[BLOCKSIZE + 1];

    for (int i = 0; i < BLOCKSIZE + 1; i++) {
        kplus[i] = 0;
    }

    if (key_len > BLOCKSIZE) {
        unsigned char newkey[HASHSIZE];
        CTX keyctx;
        md5init(&keyctx);
        md5update(&keyctx, key, key_len);
        md5padding(&keyctx);
        md5out(&keyctx, newkey);
        key_len = HASHSIZE;
        memcpy(kplus, newkey, HASHSIZE);
    } else {
        memcpy(kplus, key, key_len);
    }

    for (int i = 0; i < BLOCKSIZE; i++) {
        // 00110110
        ipad[i] = 0x36;
        // 01011100
        opad[i] = 0x5c;
    }

    unsigned char si[BLOCKSIZE + 1];
    unsigned char so[BLOCKSIZE + 1];
    for (int i = 0; i < BLOCKSIZE; i++) {
        si[i] = kplus[i] ^ ipad[i];
        so[i] = kplus[i] ^ opad[i];
    }
    si[BLOCKSIZE] = 0;
    so[BLOCKSIZE] = 0;

    CTX ctx;
    md5init(&ctx);
    md5update(&ctx, si, BLOCKSIZE);
    md5update(&ctx, input, length);
    md5padding(&ctx);
    md5out(&ctx, ref);

    md5init(&ctx);
    md5update(&ctx, so, BLOCKSIZE);
    md5update(&ctx, ref, HASHSIZE);
    md5padding(&ctx);
    md5out(&ctx, ref);

    return;
}

// int main(int argc, char const* argv[]) {
//     char c[] = "abc";
//     unsigned char ref[16];
//     // 6f1de203fbb411a19e245480f4521ffd
//     char key[] = "abcdefg";

//     hmac_md5((unsigned char*)c, strlen(c), (unsigned char*)key, strlen(key),
//              ref);

//     printf("Hmac:%d\n", strlen(c));
//     for (int i = 0; i < 16; i++) printf("%02x", ref[i]);
//     printf("\n");

//     return 0;
// }
