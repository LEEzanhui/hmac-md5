#include "stdio.h"

int main(int argc, char const *argv[]) {
    for (int i = 0; i < 70; i++) {
        printf("%d", i % 9);
    }

    return 0;
}
