#!/usr/bin/bash
gcc -O3 -Wno-unused-parameter -Wall -Wextra -D DEBUG_TIME -lm cpu.c instruction_set.c -o cpu
