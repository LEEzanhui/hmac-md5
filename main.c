/*
g++ -c md5.c -o md5.o
g++ -c hmac.c -o hmac.o
g++ -c main.c -o main.o
g++ md5.o hmac.o main.o -o main.exe
.\main.exe
*/
#include "hmac.h"
#include "md5.h"
#include "stdio.h"
#include "string.h"

#define BUFSIZE 1024

int main(int argc, char const *argv[]) {
    unsigned char ref[HASHSIZE];
    char text[BUFSIZ];
    char key[BUFSIZ];

    printf("text: \n");
    scanf("%[^\n]", text);
    getchar();

    if (strlen(text) > BUFSIZE) {
        printf("Text is too long\n");
        return 0;
    }

    printf("\nkey: \n");
    scanf("%[^\n]", key);
    getchar();

    if (strlen(key) > BUFSIZE) {
        printf("Key is too long\n");
        return 0;
    }

    hmac_md5((unsigned char *)text, strlen(text), (unsigned char *)key,
             strlen(key), ref);

    printf("\nHMAC-MD5: \n");
    for (int i = 0; i < HASHSIZE; i++) printf("%02x", ref[i]);

    return 0;
}
