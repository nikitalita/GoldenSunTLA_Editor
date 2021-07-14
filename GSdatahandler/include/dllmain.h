#ifndef DLLMAIN_H
#define DLLMAIN_H

unsigned char Version();
unsigned int FileTable();
unsigned int FileSize();
unsigned int LOAD_INT(unsigned int pos);
unsigned int READU32(FILE *filei, unsigned int pos);
void WRITE_INT(unsigned int pos, unsigned int num);
unsigned short LOAD_SHORT(unsigned int pos);
unsigned short READU16(FILE *filei, unsigned int pos);
void WRITE_SHORT(unsigned int pos, unsigned int num);
unsigned char LOAD_BYTE(unsigned int pos);
unsigned char READU8(FILE *filei, unsigned int pos);
void WRITE_BYTE(unsigned int pos, unsigned char byte);
unsigned int ror(unsigned int a, unsigned int b);

#endif // DLLMAIN_H