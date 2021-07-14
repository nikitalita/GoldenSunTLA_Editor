#ifndef STRINGS_H
#define STRINGS_H

#include "includes.h"

//EXPORTSTRING decompressString(double index);

class strings
{
public:
     static void Initialize(unsigned int R[15]);
     static unsigned short NextChar(unsigned int R[15]);
     static signed int CompressStr(unsigned char *str, unsigned char *buffer, unsigned int pos);
     static signed int NextChar_ext(unsigned int R[15], unsigned int input[2]);
};

#endif // STRINGS_H