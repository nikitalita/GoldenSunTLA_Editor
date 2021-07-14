#include "includes.h"
#include "strings.h"
#include "compression.h"
#include "spacemanager.h"

FILE * fstream;
unsigned char fileversion;
unsigned int filetable;

EXPORTREAL getVersion()
{
     return 0.5;
}

EXPORTREAL loadFile(char* name)
{
     if (fstream!=NULL)
     {
          fclose(fstream);
     }
     fstream=fopen(name, "r+b");
     if (fstream!=NULL)
     {
          if (LOAD_INT(0x080000AB)==0x53474141)
          {
               unsigned char l;
               fileversion=0;
               switch (LOAD_BYTE(0x080000AF))
               {
                    case 69: case 73: filetable=0x08320000; break;
                    case 74: filetable=0x8317000; break;
                    case 68: filetable=0x831FE00; break;
                    case 70: case 83: filetable=0x8321800; break;
               }
          }
          else if (LOAD_INT(0x080000AB)==0x474D4241)
          {
               fileversion=10; filetable=0x088000000;
          }
          else if (LOAD_INT(0x080000AB)==0x4D544241)
          {
               fileversion=11; filetable=0x08C28000;
          }
          else
          {
               fileversion=1; filetable=0x08680000;
          }
          return 1;
     }
     else
          return -1;
}

EXPORTREAL closeFile()
{
     if (fstream!=NULL)
     {
          fclose(fstream);
     }
     return 0;
}

EXPORTREAL findSpace(double size)
{
     if (fstream==NULL)
     {
          return 0;
     }
     int pos, list[32768], num, i;
     num=spacemanager::mapSpace(list);
     pos=spacemanager::findSpace(list,(int)size);
     fflush(fstream);
     return pos;
}

EXPORTREAL claimSpace(double position, double size)
{
     if (fstream==NULL)
     {
          return 0;
     }
     int list[32768], ret;
     spacemanager::mapSpace(list);
     ret = spacemanager::claimSpace(list, int(position),int(size));
     fflush(fstream);
     return (ret>0);
}

EXPORTREAL freeSpace(double position, double size)
{
     if (fstream==NULL)
     {
          return 0;
     }
     int list[32768];
     spacemanager::mapSpace(list);
     spacemanager::freeSpace(list, int(position),int(size));
     fflush(fstream);
     return 1;
}

EXPORTSTRING decompressString(double ind, double replaceNonChars, double trunc)
{
     if (fstream==NULL)
     {
          return "";
     }
     unsigned short c, n, l;
     static char str[520];
     unsigned int R[15];
     unsigned int i;
     for (i=0; i<15; i++)
     {
          R[i]=0;
     }
     R[1]=(unsigned int)(ind);
     strings::Initialize(R);
     i=0; c=0;
     do
     {
          l=c;
          c=strings::NextChar(R);
          str[i]=(unsigned char)c;
          if (replaceNonChars>0)
          {
               if (c<32 && c!=0)
               {
                    if ((i>513 && c>=100) || (i>514 && c>=10) || i>515)
                    {
                         str[i]=0; break;
                    }
                    str[i]='['; i++;
                    n=int((c/100)%10);
                    if (n!=0)
                    {
                         str[i]=48+n; i++;
                    }
                    n=int((c/10)%10);
                    if (n!=0)
                    {
                         str[i]=48+int((c/10)%10); i++;
                    }
                    n=int(c%10);
                    str[i]=48+n; i++;
                    str[i]=']';
               }
          }
          i++;
     }
     while (R[0]!=0 && i<519 && ((c!=1 && c!=3) || trunc<=0 || (l>=17 && l<=20) || l==26 || l==29));
     str[i]=0;
     return str;
}

EXPORTREAL replaceString(double ind, unsigned char* str)
{
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char buffer[262144], buffer2[1024];
     int ppos, pos, pos2, bpos, bpos2, length, length2, size, num, i, ii, psize, psize2, index;
     index=int(ind);
     bpos=0; bpos2=0; psize=0; psize2=0;
     if (Version()==0)
     {
          ppos=LOAD_INT(FileTable()+0x10);
          ppos=LOAD_INT(ppos+0x5CC);
     }
     else if (Version()==1)
     {
          ppos=LOAD_INT(0x08680010);
          ppos=LOAD_INT(ppos+0x5DC);
     }
     else if (Version()==10)
     {
          ppos=LOAD_INT(0x08800010);
          ppos=LOAD_INT(ppos+0x694);
     }
     else if (Version()==11)
     {
          ppos=LOAD_INT(0x08C28010);
          ppos=LOAD_INT(ppos+0x52C);
     }
     pos2=LOAD_INT(ppos+((index>>8)<<3));
     pos=LOAD_INT(ppos+((index>>8)<<3)+4);
     for (i=0; i<(index&0xFF); i++)
     {
          num=LOAD_BYTE(pos); pos++; psize2++;
          buffer2[bpos2]=num; bpos2++;
          size=num;
          if (num==0xFF)
          {
               num=LOAD_BYTE(pos); pos++; psize2++;
               buffer2[bpos2]=num; bpos2++;
               size+=num;
               return pos;
          }
          for (ii=0; ii<size; ii++)
          { buffer[bpos]=LOAD_BYTE(pos2); bpos++; pos2++; }
          psize+=size;
     }
     size=strings::CompressStr(str,buffer,bpos);
     if (size<=0)
     { return -2; }
     size-=bpos; bpos+=size;
     if (size>=0xFF)
     {
          buffer2[bpos2]=0xFF; bpos2++;
          buffer2[bpos2]=size-0xFF; bpos2++;
     }
     else
     { buffer2[bpos2]=size; bpos2++; }
     size=LOAD_BYTE(pos); pos++; psize2++;
     if (size==0xFF)
     { size+=LOAD_BYTE(pos); pos++; psize2++; }
     psize+=size; pos2+=size;
     for (i=(index&0xFF)+1; i<256; i++)
     {
          num=LOAD_BYTE(pos); pos++; psize2++;
          buffer2[bpos2]=num; bpos2++;
          if (num==0)
          { i=256; continue; }
          size=num;
          if (num==0xFF)
          {
               num=LOAD_BYTE(pos); pos++; psize2++;
               buffer2[bpos2]=num; bpos2++;
               size+=num;
               return pos;
          }
          for (ii=0; ii<size; ii++)
          { buffer[bpos]=LOAD_BYTE(pos2); bpos++; pos2++; }
          psize+=size;
     }
     int list[32768];
     //list++;
     num=spacemanager::mapSpace(list);
     //BEGIN DEBUG CODE
     /*FILE * log; log=fopen("log.txt","a");
     fprintf(log,"Start replacing strings, %d\n",num);
     for (i=0; i<num; i++)
     {
          fprintf(log,"%X, %X\n",list[i<<1],list[(i<<1)+1]);
     }
     fclose(log);
     //END DEBUG CODE*/
     if (bpos2==psize2)
     {
          //pos-=psize2;
          pos=LOAD_INT(ppos+((index>>8)<<3)+4);
          fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
          fwrite(buffer2,1,psize2,fstream);
     }
     else
     {
          num=spacemanager::freeSpace(list, pos-psize2, psize2);
          //DEBUG
          /*log=fopen("log.txt","a");
          fprintf(log,"1freeSpace(%X,%X)\n",pos-psize2,psize2); 
          for (i=0; i<num; i++)
          {
               fprintf(log,"%X, %X\n",list[i<<1],list[(i<<1)+1]);
          }
          fclose(log);
          //DEBUG*/
          pos=spacemanager::findSpace(list, bpos2);
          //DEBUG
          /*log=fopen("log.txt","a");
          fprintf(log,"1findSpace(%X)\n",bpos2); 
          for (i=0; i<num; i++)
          {
               fprintf(log,"%X, %X\n",list[i<<1],list[(i<<1)+1]);
          }
          fclose(log);
          //DEBUG*/
          num=spacemanager::claimSpace(list, pos, bpos2);
          if (num<0)
          { return -3; }
          //DEBUG
          /*log=fopen("log.txt","a");
          fprintf(log,"1claimSpace(%X,%X)\n",pos,bpos2); 
          for (i=0; i<num; i++)
          {
               fprintf(log,"%X, %X\n",list[i<<1],list[(i<<1)+1]);
          }
          fclose(log);
          //DEBUG*/
          fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
          fwrite(buffer2,1,bpos2,fstream);
          WRITE_INT(ppos+((index>>8)<<3)+4,pos);
     }
     if (bpos==psize)
     {
          //pos=pos2-psize;
          pos=LOAD_INT(ppos+((index>>8)<<3));
          fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
          fwrite(buffer,1,psize,fstream);
     }
     else
     {
          num=spacemanager::freeSpace(list, pos2-psize, psize);
          //DEBUG
          /*log=fopen("log.txt","a");
          fprintf(log,"2freeSpace(%X,%X)\n",pos2-psize,psize); 
          for (i=0; i<num; i++)
          {
               fprintf(log,"%X, %X\n",list[i<<1],list[(i<<1)+1]);
          }
          fclose(log);
          //DEBUG*/
          pos=spacemanager::findSpace(list, bpos);
          //DEBUG
          /*log=fopen("log.txt","a");
          fprintf(log,"2findSpace(%X)\n",bpos); 
          for (i=0; i<num; i++)
          {
               fprintf(log,"%X, %X\n",list[i<<1],list[(i<<1)+1]);
          }
          fclose(log);
          //DEBUG*/
          num=spacemanager::claimSpace(list, pos, bpos);
          if (num<=0)
          { return -4; }
          //DEBUG
          /*log=fopen("log.txt","a");
          fprintf(log,"2claimSpace(%X,%X)\n",pos,bpos); 
          for (i=0; i<num; i++)
          {
               fprintf(log,"%X, %X\n",list[i<<1],list[(i<<1)+1]);
          }
          fclose(log);
          //DEBUG*/
          fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
          fwrite(buffer,1,bpos,fstream);
          WRITE_INT(ppos+((index>>8)<<3),pos);
          //DEBUG
          /*log=fopen("log.txt","a");
          fprintf(log,"%X,%X\n",ppos+((index>>8)<<3),pos);
          fclose(log);
          //DEBUG*/
     }
     fflush(fstream);
     return bpos2;
}

EXPORTREAL compressString(char* outfile, unsigned char* str)
{
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char buffer[2048];
     int length;
     length=strings::CompressStr(str,buffer,0);
     if (length<=0)
     {
          return -2;
     }
     FILE *fstream_o;
     fstream_o=fopen(outfile, "wb");
     if (fstream_o==NULL)
      return -3;
     else
     {
          length=fwrite(buffer,1,length,fstream_o);
     }
     fclose(fstream_o);
     return length;
}

EXPORTREAL compress(char* infile, char* outfile, double format)
{
     FILE *fstream;
     fstream=fopen(infile, "r+b");
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char buffer[262144], bufferio[131072];
     int length;
     fseek(fstream,0,SEEK_END);
     length=ftell(fstream);
     if (length>131072)
     {
          fclose(fstream);
          return -4;
     }
     fseek(fstream,0,SEEK_SET);
     fread(bufferio,1,length,fstream);
     fclose(fstream); fstream=NULL;
     length=compression::compress(buffer, bufferio, length);
     //if (format!=0)
     //{
     //     length=compression::recompress(buffer, bufferio, length, (unsigned char)(format));
     //}
     fstream=fopen(outfile, "wb");
     if (fstream==NULL)
      return -3;
     else
     {
          //if (format!=0)
          //{
          //     length=fwrite(bufferio,1,length,fstream);
          //}
          //else
          //{
               length=fwrite(buffer,1,length,fstream);
          //}
     }
     fclose(fstream);
     return length;
}

EXPORTREAL decompress(double pos, double format, char* outfile)
{
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char buffer[262144];
     int length;
     length=compression::decompress(buffer, (unsigned int)(pos), (unsigned char)(format));
     if (length<=0)
     {
          return length;
     }
     if ((int(format)>>7)==0)
     {
          FILE *fstream_o;
          fstream_o=fopen(outfile, "wb");
          if (fstream_o==NULL)
           return -3;
          else
          {
               length=fwrite(buffer,1,length,fstream_o);
          }
          fclose(fstream_o);
     }
     return length;
}

EXPORTREAL decompressTileData(double pos, double format, char* outfile)
{
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char bufferI[262144];
     int length;
     length=compression::decompress(bufferI, (unsigned int)(pos), (unsigned int)(format));
     if (length<=0)
     {
          return -2;
     }
     FILE *fstream_o;
     fstream_o=fopen(outfile, "wb");
     if (fstream_o==NULL)
      return -3;
     else
     {
          unsigned char buffer[262144];
          unsigned int i, pos1, pos2, n=0;
          switch (bufferI[0])
          {
               case 1:
                    pos2=(length>>1)+1;
                    i=0;
                    for(pos1=1; pos1<(length>>1); pos1++)
                    {
                         n=n^((bufferI[pos1]<<8)|(bufferI[pos2]));
                         buffer[i]=n&255; pos2++;
                         buffer[i+1]=(n>>8)&255; i+=2;
                    }
                    length--;
               break;
               case 2:
                    for (i=1; i<(length-1); i+=2)
                    {
                         n^=bufferI[i]|(bufferI[i+1]<<8);
                         buffer[i-1]=(n&255); buffer[i]=((n>>8)&255);
                    }
                    length--;
               break;
               default:
                    for (i=0; i<length; i++)
                    {
                         buffer[i]=bufferI[i+1];
                    }
                    length--;
               break;
          }
          length=fwrite(buffer,1,length,fstream_o);
     }
     fclose(fstream_o);
     return length;
}

EXPORTREAL decompressMap(double pos, double format, char* outfile)
{
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char bufferI[262144];
     int length;
     length=compression::decompress(bufferI, (unsigned int)(pos), (unsigned int)(format));
     if (length<=0)
     {
          return -2;
     }
     FILE *fstream_o;
     fstream_o=fopen(outfile, "wb");
     if (fstream_o==NULL)
      return -3;
     else
     {
          unsigned char buffer[262144];
          unsigned int a, b, num, pos1, pos2, pos3;
          a=0xFFF00000;
          b=0xFFFFF001;
          pos2=0;
          for (pos1=0x8000; pos1<0x10000; pos1+=2)
          {
               num=bufferI[pos1]|(bufferI[pos1+1]<<8);
               if ((num<<20)==a)
               {
                    num+=b;
                    if (b!=0)
                    {
                         b++;
                    }
               }
               buffer[pos2]=num&255; buffer[pos2+1]=(num>>8)&255; pos2+=4;
          }
          pos2=0x4000;
          pos3=2;
          for (pos1=0; pos1<0x4000; pos1++)
          {
               buffer[pos3]=bufferI[pos1]; buffer[pos3+1]=bufferI[pos2]; pos2++; pos3+=4;
          }
          length=fwrite(buffer,1,length,fstream_o);
     }
     fclose(fstream_o);
     return length;
}

EXPORTREAL decompressMap2(double pos, double format, char* outfile)
{
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char buffer[262144];
     int length;
     length=compression::decompress(buffer, (unsigned int)(pos), (unsigned int)(format));
     if (length<=0)
     {
          return -2;
     }
     FILE *fstream_o;
     fstream_o=fopen(outfile, "wb");
     if (fstream_o==NULL)
      return -3;
     else
     {
          unsigned int a, b, num, pos1, pos2, pos3;
          a=0xFFF00000;
          b=0xFFFFF001;
          for (pos1=0x8000; pos1<0x10000; pos1+=2)
          {
               num=buffer[pos1]|(buffer[pos1+1]<<8);
               if ((num<<20)==a)
               {
                    num+=b;
                    if (b!=0)
                    {
                         b++;
                    }
               }
               buffer[pos1]=num&255; buffer[pos1+1]=(num>>8)&255;
          }
          length=fwrite(buffer,1,length,fstream_o);
     }
     fclose(fstream_o);
     return length;
}

EXPORTREAL decompress16(double pos, char* outfile, double size, double pal)
{
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char bufferI[262144];
     int length;
     length=compression::decompress16(bufferI, (unsigned int)(pos));
     if (length<=0)
     {
          return -2;
     }
     FILE *fstream_o;
     fstream_o=fopen(outfile, "wb");
     if (fstream_o==NULL)
      return -3;
     else
     {
          unsigned char buffer[262144];
          unsigned int i, ii, wid, hig, dy;
          wid=(unsigned int)size;
          hig=(unsigned int)size;
          unsigned short num;
          length=length>>1;
          buffer[0]=66; buffer[1]=77;
          buffer[2]=(length+74)&255; buffer[3]=((length+74)>>8)&255; buffer[4]=((length+74)>>16)&255; buffer[5]=((length+74)>>24)&255;
          buffer[6]=0; buffer[7]=0;
          buffer[8]=0; buffer[9]=0;
          buffer[10]=74; buffer[11]=0; buffer[12]=0; buffer[13]=0;
          
          buffer[14]=12; buffer[15]=0; buffer[16]=0; buffer[17]=0;
          buffer[18]=wid&255; buffer[19]=(wid>>8)&255;
          buffer[20]=(hig+1)&255; buffer[21]=((hig+1)>>8)&255;
          buffer[22]=1; buffer[23]=0;
          buffer[24]=4; buffer[25]=0;
          
          for (i=0; i<16; i++)
          {
               num=LOAD_SHORT((unsigned int)pal+(i<<1));
               buffer[26+i*3]=((num>>10)&31)<<3;
               buffer[27+i*3]=((num>>5)&31)<<3;
               buffer[28+i*3]=((num>>0)&31)<<3;
          }
          buffer[26]=255; buffer[27]=255; buffer[28]=255;
          
          for (i=0; i<(wid>>1); i++)
          { buffer[74+i]=0; }
          
          for (i=1; i<=hig; i++)
          {
               dy=((hig)-i)*wid;
               for (ii=0; ii<(wid>>1); ii++)
               {
                    buffer[74+ii+i*(wid>>1)]=(bufferI[(ii<<1)+1+dy]&15)|((bufferI[(ii<<1)+dy]&15)<<4);
               }
          }
          length=fwrite(buffer,1,length+(wid>>1)+74,fstream_o);
     }
     fclose(fstream_o);
     return length;
}

EXPORTREAL createTileset(char* tilefile, char* palfile, char* outfile)
{
     FILE *fstream_o;
     fstream_o=fopen(outfile, "wb");
     if (fstream_o==NULL)
      return -3;  
     
     unsigned char buffer[524288];
     
     unsigned int length, wid=512, hig=897;
     length=wid*hig+794;
     
     buffer[0]=66; buffer[1]=77;
     buffer[2]=(length)&255; buffer[3]=(length>>8)&255; buffer[4]=(length>>16)&255; buffer[5]=(length>>24)&255;
     buffer[6]=0; buffer[7]=0;
     buffer[8]=0; buffer[9]=0;
     buffer[10]=26; buffer[11]=3; buffer[12]=0; buffer[13]=0;
     
     buffer[14]=12; buffer[15]=0; buffer[16]=0; buffer[17]=0;
     buffer[18]=wid&255; buffer[19]=(wid>>8)&255;
     buffer[20]=(hig)&255; buffer[21]=((hig)>>8)&255;
     buffer[22]=1; buffer[23]=0;
     buffer[24]=8; buffer[25]=0;
     
     FILE *fstream_i;
     fstream_i=fopen(palfile, "rb");
     if (fstream_i==NULL)
     {
          fclose(fstream_o);
          return -2;
     }
     
     unsigned int i, ix, iy, dx, dy, num, p;
     unsigned char byte;
     
     for (i=0; i<256; i++)
     {
          if ((i&15)==0)
          {
               fread(&byte,1,1,fstream_i);
               fread(&byte,1,1,fstream_i);
               buffer[26+i*3]=255;
               buffer[27+i*3]=255;
               buffer[28+i*3]=255;
          }
          else
          {
               fread(&byte,1,1,fstream_i); num=byte;
               fread(&byte,1,1,fstream_i); num|=(byte<<8);
               buffer[26+i*3]=((num>>10)&31)<<3;
               buffer[27+i*3]=((num>>5)&31)<<3;
               buffer[28+i*3]=((num>>0)&31)<<3;
          }    
     }
     
     fclose(fstream_i);
     
     for (i=0; i<wid; i++)
     { buffer[794+i]=0; }
     
     fstream_i=fopen(tilefile, "rb");
     if (fstream_i==NULL)
     {
          fclose(fstream_o);
          return -1;
     }
     
     for (i=0; i<512; i++)
     {
          ix=(i&31)<<3;
          ix+=((15-(i>>5))<<3)<<9;
          for (dy=0; dy<8; dy++)
          {
               iy=(7-dy)<<9;
               num=(dy<<2)+(i<<5);
               for (dx=0; dx<8; dx+=2)
               {
                    fseek(fstream_i, (dx>>1)+num, SEEK_SET);
                    fread(&byte,1,1,fstream_i);
                    buffer[1306+ix+dx+iy]=(byte&15);
                    buffer[1307+ix+dx+iy]=(byte>>4);
               }
          }
     }
     
     for (p=1; p<14; p++)
     {
          ix=(p&1)<<8;
          ix+=((p>>1)<<7)<<9;
          iy=(p<<4);
          for (dy=0; dy<128; dy++)
          {
               dy=1306+(dy<<9);
               for (dx=0; dx<256; dx++)
               {
                    num=dx+dy;
                    buffer[num+ix]=buffer[num]|iy;
               }
               dy=(dy-1306)>>9;
          }
     }
     
     /*for (i=1; i<=hig; i++)
     {
          dy=(127-((i-1)&127))*128;
          for (ii=0; ii<wid; ii+=2)
          {
               p=(((i>>7)<<1)|(ii>>8))<<4;
               fseek(fstream_i, ((ii&255)>>1)+dy, SEEK_SET);
               fread(&byte,1,1,fstream_i);
               buffer[794+ii+i*wid]=(byte&15)|p;
               buffer[795+ii+i*wid]=(byte>>4)|p;
          }
     }*/
     
     fclose(fstream_i);
     
     length=fwrite(buffer,1,length,fstream_o);
     
     fclose(fstream_o);
     return length;
}

/*EXPORTREAL convertMap(char* mapfile, char* tilefile, char* outfile)
{
     FILE *fstream_o;
     fstream_o=fopen(outfile, "wb");
     if (fstream_o==NULL)
      return -3;  
     
     unsigned char buffer[262144];
     unsigned int length=262144;
     
     FILE *fstream_m;
     fstream_m=fopen(mapfile, "rb");
     if (fstream_m==NULL)
     {
          fclose(fstream_o);
          return -2;
     }
     FILE *fstream_t;
     fstream_t=fopen(tilefile, "rb");
     if (fstream_m==NULL)
     {
          fclose(fstream_o);
          fclose(fstream_m);
          return -1;
     }
     
     unsigned int cx, cy, c, num, t;
     unsigned char byte;
     for (cy=0; cy<128; cy++)
     {
          for (cx=0; cx<128; cx++)
          {
               fread(&byte,1,1,fstream_m); num=byte;
               fread(&byte,1,1,fstream_m); num|=(byte<<8);
               fread(&byte,1,1,fstream_m); num|=(byte<<16);
               fread(&byte,1,1,fstream_m); num|=(byte<<24);
               t=(num&1023);
               fseek(fstream_t, t<<3, SEEK_SET);
               c=(cx<<2)+(cy<<9);
               fread(buffer+c,1,1,fstream_t);
               fread(buffer+c+1,1,1,fstream_t);
               fread(buffer+c+2,1,1,fstream_t);
               fread(buffer+c+3,1,1,fstream_t);
               fread(buffer+c+512,1,1,fstream_t);
               fread(buffer+c+513,1,1,fstream_t);
               fread(buffer+c+514,1,1,fstream_t);
               fread(buffer+c+515,1,1,fstream_t);
          }
     }     
     
     length=fwrite(buffer,1,length,fstream_o);
     
     fclose(fstream_o);
     return length;
}*/
 
EXPORTREAL exportFont(char* outfile)
{
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char buffer[8192];
     buffer[0]=66; buffer[1]=77;
     buffer[2]=204; buffer[3]=31; buffer[4]=0; buffer[5]=0;
     buffer[6]=0; buffer[7]=0;
     buffer[8]=0; buffer[9]=0;
     buffer[10]=74; buffer[11]=0; buffer[12]=0; buffer[13]=0;
     
     buffer[14]=12; buffer[15]=0; buffer[16]=0; buffer[17]=0;
     buffer[18]=0; buffer[19]=7;
     buffer[20]=9; buffer[21]=0;
     buffer[22]=1; buffer[23]=0;
     buffer[24]=4; buffer[25]=0;
     
     buffer[26]=0; buffer[27]=128; buffer[28]=0;
     buffer[29]=240; buffer[30]=240; buffer[31]=240;
     buffer[32]=0; buffer[33]=0; buffer[34]=0;
     
     
     unsigned char dx, dy, byte, byte2;
     unsigned int i;
     
     for (i=35; i<74; i++)
     { buffer[i]=0; }
     if (Version()==0)
     { fseek(fstream,(LOAD_INT(FileTable()+0x4C)+0x420)&0x1FFFFFF,SEEK_SET); }
     else if (Version()==1)
     { fseek(fstream,(LOAD_INT(0x0868004C)+0x420)&0x1FFFFFF,SEEK_SET); }
     else if (Version()==10)
     { fseek(fstream,(LOAD_INT(0x08800048)+0x420)&0x1FFFFFF,SEEK_SET); }
     else if (Version()==11)
     { fseek(fstream,(LOAD_INT(0x08C28078)+0x420)&0x1FFFFFF,SEEK_SET); }
     for (i=0; i<224; i++)
     {
          for (dy=0; dy<8; dy++)
          {
               for(dx=0; dx<4; dx++)
               {
                    byte=0;
                    fread(&byte2,1,1,fstream);
                    byte=byte2&3;
                    if (byte==2)
                    { byte=0; }
                    else if (byte==3)
                    { byte=2; }
                    byte2=(byte2>>4)&3;
                    if (byte2==2)
                    { byte2=0; }
                    else if (byte2==3)
                    { byte2=2; }
                    byte=byte2|(byte<<4);
                    buffer[74+896*(8-dy)+dx+(i<<2)]=byte;
               }
          }
     }
     
     for (i=74; i<970; i++)
     { buffer[i]=0; }
     
     FILE *fstream_o;
     fstream_o=fopen(outfile, "wb");
     if (fstream_o==NULL)
      return -2;
     else
     {
          i=fwrite(buffer,1,8140,fstream_o);
     }
     fclose(fstream_o);
     if (i!=0)
     { return 1; }
     else
     { return -3; }
}

#define BSIZE 1024

EXPORTREAL fileExtract(char* infile, char* outfile, double pos, double len)
{
    int length; //Used to count bytes to read
    char buffer[BSIZE]; //Data buffer
    FILE *infstream; //Input file stream
    FILE *outfstream; //Output file stream
    
    //Open the input file, if an error occurs abort and return it.
    infstream=fopen(infile, "rb");
    if (infstream==NULL)
    {   return -1; }
    //Open the output file, if an error occurs abort and return it.
    outfstream=fopen(outfile, "wb");
    if (outfstream==NULL)
    {
        fclose(infstream);
        return -2;
    }
    
    //Go the the starting position in the input file.
    fseek(infstream,(int)pos, SEEK_SET);
    
    //Begin copying.
    while (len>0)
    {
        //First find out if we'll be copying the entire size of the buffer
        //this round, or what ever is left over.
        length=(int)len;
        if (length>BSIZE)
        {   length=BSIZE; }
        
        //Read the desired amount into the buffer from the input file.
        fread(&buffer,1,length,infstream);
        //On failure close the files, and return an error value.
        if (ferror(infstream))
        {
            fclose(outfstream);
            fclose(infstream);
            return -3;
        }
        //Write the buffer to the output file.
        fwrite(&buffer,1,length,outfstream);
        //On failure close the files, and return an error value.
        if (ferror(outfstream))
        {
            fclose(outfstream);
            fclose(infstream);
            return -4;
        }
        
        //Subtract the amount we just read from the total amount left to be read
        len-=length;
    }
    
    //Close both files, and return success.
    fclose(outfstream);
    fclose(infstream);
    return 1;
}

EXPORTREAL insertFile(char* infile, char* outfile, double posWrite, double len)
{
    int length; //Used to count bytes to read
    char buffer[BSIZE]; //Data buffer
    FILE *infstream; //Input file stream
    FILE *outfstream; //Output file stream
    
    //Open the input file, if an error occurs abort and return it.
    infstream=fopen(infile, "rb");
    if (infstream==NULL)
    {   return -1; }
    //Open the output file, if an error occurs abort and return it.
    outfstream=fopen(outfile, "r+b");
    if (outfstream==NULL)
    {
        fclose(infstream);
        return -2;
    }
    
    //Figure out how much we're writing
    if (len<=0)
    {
        fseek(infstream, 0, SEEK_END);
        len=ftell(infstream);
    }
    
    rewind(infstream);
    //Go to the writing position in the output file.
    if (posWrite<0)
    {
        fseek(outfstream, 0, SEEK_END);      
    }
    else
    {
        fseek(outfstream,(int)posWrite, SEEK_SET);
    }
    
    //Begin copying.
    while (len>0)
    {
        //First find out if we'll be copying the entire size of the buffer
        //this round, or what ever is left over.
        length=(int)len;
        if (length>BSIZE)
        {   length=BSIZE; }
        
        //Read the desired amount into the buffer from the input file.
        fread(&buffer,1,length,infstream);
        //On failure close the files, and return an error value.
        if (ferror(infstream))
        {
            fclose(outfstream);
            fclose(infstream);
            return -3;
        }
        //Write the buffer to the output file.
        fwrite(&buffer,1,length,outfstream);
        //On failure close the files, and return an error value.
        if (ferror(outfstream))
        {
            fclose(outfstream);
            fclose(infstream);
            return -4;
        }
        
        //Subtract the amount we just read from the total amount left to be read
        len-=length;
    }
    
    //Close both files, and return success.
    fclose(outfstream);
    fclose(infstream);
    return 1;
}

unsigned char Version()
{
     return fileversion;
}

unsigned int FileTable()
{
     return filetable;
}

unsigned int FileSize()
{
     fseek(fstream,0,SEEK_END);
     return ftell(fstream);
}

unsigned int LOAD_INT(unsigned int pos)
{
     unsigned int num;
     unsigned char byte;
     fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
     fread(&byte,1,1,fstream); num=byte;
     fread(&byte,1,1,fstream); num|=(byte<<8);
     fread(&byte,1,1,fstream); num|=(byte<<16);
     fread(&byte,1,1,fstream); num|=(byte<<24);
     return num;
}

unsigned int READU32(FILE *filei, unsigned int pos)
{
     unsigned int num;
     unsigned char byte;
     fseek(filei, pos&0x1FFFFFF, SEEK_SET);
     fread(&byte,1,1,filei); num=byte;
     fread(&byte,1,1,filei); num|=(byte<<8);
     fread(&byte,1,1,filei); num|=(byte<<16);
     fread(&byte,1,1,filei); num|=(byte<<24);
     return num;
}

void WRITE_INT(unsigned int pos, unsigned int num)
{
     unsigned char byte;
     fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
     byte=(num&0xFF); fwrite(&byte,1,1,fstream);
     byte=((num>>8)&0xFF); fwrite(&byte,1,1,fstream);
     byte=((num>>16)&0xFF); fwrite(&byte,1,1,fstream);
     byte=((num>>24)&0xFF); fwrite(&byte,1,1,fstream);
     return;
}

unsigned short LOAD_SHORT(unsigned int pos)
{
     unsigned short num;
     unsigned char byte;
     fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
     fread(&byte,1,1,fstream); num=byte;
     fread(&byte,1,1,fstream); num|=(byte<<8);
     return num;
}

void WRITE_SHORT(unsigned int pos, unsigned int num)
{
     unsigned char byte;
     fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
     byte=(num&0xFF); fwrite(&byte,1,1,fstream);
     byte=((num>>8)&0xFF); fwrite(&byte,1,1,fstream);
     return;
}

unsigned short READU16(FILE *filei, unsigned int pos)
{
     unsigned short num;
     unsigned char byte;
     fseek(filei, pos&0x1FFFFFF, SEEK_SET);
     fread(&byte,1,1,filei); num=byte;
     fread(&byte,1,1,filei); num|=(byte<<8);
     return num;
}

unsigned char LOAD_BYTE(unsigned int pos)
{
     unsigned char num;
     fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
     fread(&num, 1, 1, fstream);
     return num;
}

unsigned char READU8(FILE *filei, unsigned int pos)
{
     unsigned char num;
     fseek(filei, pos, SEEK_SET);
     fread(&num, 1, 1, filei);
     return num;
}

void WRITE_BYTE(unsigned int pos, unsigned char byte)
{
     fseek(fstream, pos&0x1FFFFFF, SEEK_SET);
     fwrite(&byte,1,1,fstream);
     return;
}

unsigned int ror(unsigned int a, unsigned int b)
{
     return ((a>>b)|((a&((1<<b)-1))<<(32-b)));
}
