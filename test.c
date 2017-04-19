#include <stdio.h>
#include <stdint.h>

int main(){
    printf("Sizeof size_t: %d\n", sizeof(size_t));
    printf("Sizeof char: %d\n", sizeof(char));
    printf("Sizeof uint: %d\n", sizeof(unsigned int));
    printf("Sizeof uint16_t *: %d\n", sizeof(uint16_t *));
    printf("Sizeof uint16_t: %d\n", sizeof(uint16_t));
}
