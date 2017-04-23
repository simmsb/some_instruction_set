#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

uint16_t hex_value(char val) {
  switch (val) {
    case '0' ... '9':
      return val - '0';
    case 'A' ... 'F':
      return 10 + (val - 'A');
    case 'a' ... 'f':
      return 10 + (val - 'a');
    default:
      return 0;
  }
}

int main(int argc, char **argv) {
  char *vals = "0123456789abcdef"-1;
  while (*vals++)
    printf("%c is %d\n", *vals, hex_value(*vals));
}
