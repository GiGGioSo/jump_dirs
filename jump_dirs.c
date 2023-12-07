#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    if (argc <= 1) {
        printf("No arguments!\n");
        exit(1);
    } else {
        printf("%s\n", argv[1]);
    }
    exit(0);
}
