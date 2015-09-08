#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
        char *pointer = NULL;
        char array[10];
        pointer = array;
        
        strcpy(pointer, argv[1]);
        printf("Array contains %s at %p\n", pointer, &pointer);
        strcpy(pointer, argv[2]);
        printf("Array contains %s at %p\n", pointer, &pointer);

        return EXIT_SUCCESS;
}
