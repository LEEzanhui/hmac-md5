typedef struct {
    // 计数数据的bit数
    unsigned int num[2];
    // A,B,C,D
    unsigned int state[4];
    // 数据缓存区
    unsigned char buffer[64];
} CTX;

void md5init(CTX* ctx);
void md5update(CTX* ctx, const unsigned char* input, unsigned int length);
void md5padding(CTX* ctx);
void md5out(CTX* ctx, unsigned char* ref);

void md5compress(CTX* ctx, const unsigned char* block);
void int_char(unsigned char* output, const unsigned int* input,
              unsigned int length);
void char_int(unsigned int* output, const unsigned char* input,
              unsigned int length);