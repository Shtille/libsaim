#!/bin/sh
g++ main.cpp -std=c++11 -DSAIM_STATICLIB -L../../../lib -lsaim -lcurl -ljpeg -lpng -lz -o app