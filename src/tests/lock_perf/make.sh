#!/bin/sh
gcc main.c -std=c11 -O3 -DSAIM_STATICLIB -L../../../lib -lsaim -lcurl -ljpeg -lpng -lz -o app