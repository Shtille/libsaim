gcc main.c -g -O3 -DSAIM_STATICLIB -L../../../bin -lsaim -lcurl -ljpeg -lpng -lz -lWs2_32 -lWldap32 -o app