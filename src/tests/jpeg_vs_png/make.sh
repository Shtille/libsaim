#!/bin/sh
gcc main.c -O3 -DSAIM_STATICLIB -L../../../lib -lsaim -lcurl -ljpeg -lpng -lz -o app