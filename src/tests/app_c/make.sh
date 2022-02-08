#!/bin/sh
gcc main.c -DSAIM_STATICLIB -L../../../lib -lsaim -lcurl -ljpeg -lpng -lz -o app