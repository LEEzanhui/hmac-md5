#include "md5.h"

#include "stdio.h"
#include "string.h"

unsigned int k[] = {0,  1,  2,  3, 4, 5,  6,  7,  8,  9,  10, 11, 12,
                    13, 14, 15, 1, 6, 11, 0,  5,  10, 15, 4,  9,  14,
                    3,  8,  13, 2, 7, 12, 5,  8,  11, 14, 1,  4,  7,
                    10, 13, 0,  3, 6, 9,  12, 15, 2,  0,  7,  14, 5,
                    12, 3,  10, 1, 8, 15, 6,  13, 4,  11, 2,  9};

// 用于循环左移
unsigned int S[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                    5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
                    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

unsigned int T[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
    0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
    0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
    0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
    0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
    0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

// 填充
unsigned char padding[] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// 轮函数
unsigned int F(unsigned int x, unsigned int y, unsigned int z) {
    return ((x & y) | (~x & z));
}
unsigned int G(unsigned int x, unsigned int y, unsigned int z) {
    return ((x & z) | (y & ~z));
}
unsigned int H(unsigned int x, unsigned int y, unsigned int z) {
    return (x ^ y ^ z);
}
unsigned int I(unsigned int x, unsigned int y, unsigned int z) {
    return (y ^ (x | ~z));
}

// 将32位输入循环左移
unsigned int rotateLeft(unsigned int *x, unsigned int n) {
    return ((*x) << (n) | (*x) >> (32 - n));
}

// 一次迭代运算逻辑
void FF(unsigned int *a, unsigned int b, unsigned int c, unsigned int d,
        unsigned int x, unsigned int s, unsigned int T) {
    (*a) += F(b, c, d) + x + T;
    (*a) = rotateLeft(a, s);
    (*a) += b;
}

void GG(unsigned int *a, unsigned int b, unsigned int c, unsigned int d,
        unsigned int x, unsigned int s, unsigned int T) {
    (*a) += G(b, c, d) + x + T;
    (*a) = rotateLeft(a, s);
    (*a) += b;
}

void HH(unsigned int *a, unsigned int b, unsigned int c, unsigned int d,
        unsigned int x, unsigned int s, unsigned int T) {
    (*a) += H(b, c, d) + x + T;
    (*a) = rotateLeft(a, s);
    (*a) += b;
}

void II(unsigned int *a, unsigned int b, unsigned int c, unsigned int d,
        unsigned int x, unsigned int s, unsigned int T) {
    (*a) += I(b, c, d) + x + T;
    (*a) = rotateLeft(a, s);
    (*a) += b;
}

// 字符串复制
void charcpy(unsigned char *output, const unsigned char *input, int length) {
    for (int i = 0; i < length; i++) {
        output[i] = input[i];
    }
}

// md5初始化
void md5init(CTX *ctx) {
    // 初始化A,B,C,D
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    // 清零计数器
    ctx->num[0] = 0;
    ctx->num[1] = 0;
}

// md5处理
void md5update(CTX *ctx, const unsigned char *input, unsigned int length) {
    unsigned int index = 0;
    unsigned int left = 0;
    // bit / 8 即为 byte, mod 64,本质是切分数据为512位一组
    index = (ctx->num[0] / 8) & 0x3F;
    // 计算剩余字节空间
    left = 64 - index;

    // 计数器累加
    ctx->num[0] += length * 8;
    ctx->num[1] += length >> 29;
    // 处理可能的溢出
    if (ctx->num[0] < (length * 8)) {
        ctx->num[1]++;
    }

    unsigned int i;
    // 512位一组进行处理
    if (length >= left) {
        charcpy(&ctx->buffer[index], input, left);
        // 处理上次剩余的加上这次补充的,填满一组
        md5compress(ctx, ctx->buffer);
        // 处理这次处理剩下的输入,直至输入不足一组
        for (i = left; i + 64 < length + 1; i += 64) {
            md5compress(ctx, &input[i]);
        }
        index = 0;
    } else {
        i = 0;
    }

    // 缓存剩余输入(不满一组)
    charcpy(&ctx->buffer[index], &input[i], length - i);
}

// 数据填充
void md5padding(CTX *ctx) {
    unsigned char lInfo[8];
    unsigned int index, left;

    // 获得数据长度
    int_char(lInfo, ctx->num, 8);

    // 增加padding
    index = (ctx->num[0] / 8) & 0x3F;
    if (index < 56) {
        left = 56 - index;
    } else {
        left = 120 - index;
    }
    md5update(ctx, padding, left);

    // 增加长度信息
    md5update(ctx, lInfo, 8);
}

// 拼接输出
void md5out(CTX *ctx, unsigned char *ref) {
    // 获取MD5码,存入ref中
    int_char(ref, ctx->state, 16);
}

// H_MD5
void md5compress(CTX *ctx, const unsigned char *block) {
    unsigned int A = ctx->state[0];
    unsigned int B = ctx->state[1];
    unsigned int C = ctx->state[2];
    unsigned int D = ctx->state[3];
    unsigned int X[16];

    // 将64字节的一组数据划分为16个子分组
    char_int(X, block, 64);

    // 注意每次迭代后要做循环置换
    //第1轮循环逻辑
    int i = 0;
    for (; i < 16; i++) {
        FF(&A, B, C, D, X[k[i]], S[i], T[i]);
        unsigned int mid = A;
        A = D;
        D = C;
        C = B;
        B = mid;
    }

    //第2轮循环逻辑
    for (; i < 32; i++) {
        GG(&A, B, C, D, X[k[i]], S[i], T[i]);
        unsigned int mid = A;
        A = D;
        D = C;
        C = B;
        B = mid;
    }

    //第3轮循环逻辑
    for (; i < 48; i++) {
        HH(&A, B, C, D, X[k[i]], S[i], T[i]);
        unsigned int mid = A;
        A = D;
        D = C;
        C = B;
        B = mid;
    }

    //第4轮循环逻辑
    for (; i < 64; i++) {
        II(&A, B, C, D, X[k[i]], S[i], T[i]);
        unsigned int mid = A;
        A = D;
        D = C;
        C = B;
        B = mid;
    }

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}

void int_char(unsigned char *text, const unsigned int *input,
              unsigned int length) {
    unsigned int i = 0;
    unsigned int j = 0;

    // 32位分为4组，每组转成一个unsigned char
    for (; j < length; i++, j += 4) {
        text[j] = (unsigned char)(input[i] & 0xff);
        text[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        text[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        text[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

// encode的逆过程
void char_int(unsigned int *output, const unsigned char *text,
              unsigned int length) {
    unsigned int i = 0;
    unsigned int j = 0;
    for (; j < length; i++, j += 4) {
        output[i] = ((unsigned int)text[j]) |
                    (((unsigned int)text[j + 1]) << 8) |
                    (((unsigned int)text[j + 2]) << 16) |
                    (((unsigned int)text[j + 3]) << 24);
    }
}

// int main() {
//     CTX md5ctx;
//     char c[] = "abc";
//     unsigned char md5[16];
//     // 900150983cd24fb0d6963f7d28e17f72

//     md5init(&md5ctx);
//     md5update(&md5ctx, (unsigned char *)c, strlen(c));
//     md5padding(&md5ctx);
//     md5out(&md5ctx, md5);

//     printf("md5:%d\n", strlen(c));
//     for (int i = 0; i < 16; i++) printf("%02x", md5[i]);
//     printf("\n");
//     return 0;
// }
