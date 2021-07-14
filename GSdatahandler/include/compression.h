#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "includes.h"

class compression
{
public:
     static int compress(unsigned char *buffer, unsigned char *bufferi, unsigned int filesize);
     //static int compress_bup(unsigned char *buffer, unsigned char *bufferi, unsigned int filesize);
     static int recompress(unsigned char *buffer, unsigned char *buffero, unsigned int filesize, unsigned char format);
      static void compress1_byte();
     static int decompress(unsigned char *buffer, unsigned int pos, unsigned char format);
     static int decompress16(unsigned char *buffer, unsigned int pos);
};

#endif // COMPRESSION_H