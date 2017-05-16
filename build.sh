#!/usr/bin/bash
gcc -O3 -Wno-unused-parameter -Wall -Wextra -D DEBUG_TIME cpu.c instruction_set.c -o cpu