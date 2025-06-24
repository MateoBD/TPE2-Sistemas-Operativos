#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Compiling gna memory manager"
    gcc -Wall -pedantic -fsanitize=address main.c buddymm.c memory-manager.c test_mm.c test_util.c &> /dev/null
else
    echo "Compiling buddy memory manager"
    gcc -Wall -pedantic -fsanitize=address main.c buddymm.c memory-manager.c test_mm.c test_util.c -D $1 &> /dev/null
fi