#define BLOCKSIZE 64
#define HASHSIZE 16

void hmac_md5(const unsigned char* input, unsigned int length,
              const unsigned char* key, unsigned int key_len,
              unsigned char* ref);
