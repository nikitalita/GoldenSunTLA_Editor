#include "strings.h"

/*EXPORTSTRING decompressString(double ind, double replaceNonChars, double trunc)
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

EXPORTREAL compressString(char* outfile, char* str)
{
     if (fstream==NULL)
     {
          return -1;
     }
     unsigned char buffer[2048];
     int length;
     length=strings::CompressStr(str, buffer, 0);
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
}*/

void strings::Initialize(unsigned int R[15])
{
     bool C;
     if (Version()==0)
     {
          R[12]=LOAD_INT(FileTable()+0x10);
          R[12]=LOAD_INT(R[12]+0x5CC);
     }
     else if (Version()==1)
     {
          R[12]=LOAD_INT(0x08680010);
          R[12]=LOAD_INT(R[12]+0x5DC);
     }
     else if (Version()==10)
     {
          R[12]=LOAD_INT(0x08800010);
          R[12]=LOAD_INT(R[12]+0x694);
     }
     else if (Version()==11)
     {
          R[12]=LOAD_INT(0x08C28010);
          R[12]=LOAD_INT(R[12]+0x52C);
     }
     R[3]=R[1]>>8;
     R[12]+=R[3]<<3;
     R[2]=LOAD_INT(R[12]); R[4]=LOAD_INT(R[12]+4);
     R[1]=R[1]&0xFF;
     if (R[1]==0)
          goto _080155A0;
_08015588:
     R[3]=LOAD_BYTE(R[4]); R[4]++;
     R[2]+=R[3];
     if (R[3]==0xFF)
          goto _08015588;
     R[1]--;
     if (R[1]!=0)
          goto _08015588;
_080155A0:
     R[3]=1;
     R[4]=R[2]&3;
     if (R[4]==0)
          goto _080155C0;
     R[4]=(R[4]<<3)-R[3];
     R[2]=(R[2]|3)^3;
     R[3]=LOAD_INT(R[2]); R[2]+=4;
     R[3]=(R[3]>>1)|(1<<31);
     R[3]=R[3]>>R[4];
_080155C0:
     return;
}

unsigned short strings::NextChar(unsigned int R[15])
{
     bool C;
     if (Version()==0)
     {
          R[12]=LOAD_INT(FileTable()+0x10);
          R[12]=LOAD_INT(R[12]+0x56C);
     }
     else if (Version()==1)
     {
          R[12]=LOAD_INT(0x08680010);
          R[12]=LOAD_INT(R[12]+0x578);
     }
     else if (Version()==10)
     {
          R[12]=LOAD_INT(0x08800010);
          R[12]=LOAD_INT(R[12]+0x628);
     }
     else if (Version()==11)
     {
          R[12]=LOAD_INT(0x08C28010);
          R[12]=LOAD_INT(R[12]+0x4A4);
     }
     R[4]=R[1]>>8;
     R[12]+=R[4]<<3;
     R[4]=LOAD_INT(R[12]); R[5]=LOAD_INT(R[12]+4);
     R[12]=R[1]&0xFF;
     R[12]+=R[12];
     R[5]=LOAD_SHORT(R[5]+R[12]);
     R[4]+=R[5];
     R[5]=R[4];
     R[12]=1;
     R[6]=R[4]&3;
     if (R[6]==0)
          goto _08015480;
     R[6]=(R[6]<<3)-R[12];
     R[4]=(R[4]|3)^3;
     R[12]=LOAD_INT(R[4]); R[4]+=4;
     R[12]=(R[12]>>1)|(1<<31);
     R[12]=R[12]>>R[6];
     R[6]=0;
_08015480:
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[4]); R[4]+=4;
          C=(R[12]&1)==1; R[12]=(R[12]>>1)|(1<<31);
     }
     if (C)
          goto _08015538;
     C=(R[3]&1)==1; R[3]=R[3]>>1;
     if (!C)
          goto _08015480;
     if (R[3]==0)
     {
          R[3]=LOAD_INT(R[2]); R[2]+=4;
          C=(R[3]&1)==1; R[3]=(R[3]>>1)|(1<<31);
     }
     if (!C)
          goto _08015480;
     R[1]=0;
_080154A8:
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (C)
          goto _08015510;
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (C)
          goto _080154D4;
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (C)
          goto _080154D0;
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (C)
          goto _080154F0;
     R[1]+=4;
     goto _080154A8;
_080154D0:
     R[1]++;
_080154D4:
     if (R[12]!=0)
     {
          R[6]++;
          goto _080154A8;
     }
     R[12]=LOAD_INT(R[4]); R[4]+=4;
     C=(R[12]&1)==1; R[12]=(R[12]>>1)|(1<<31);
     if (!C)
          R[1]+=2;
     if (C)
          R[6]++;
     goto _080154A8;
_080154F0:
     R[1]+=2;
     goto _080154D4;
_08015510:
     if (R[12]==0)
          goto _08015524;
_08015514:
     R[6]++;
     R[1]--;
     if ((R[1]>>31)==0)
          goto _080154A8;
     goto _08015480;
_08015524:
     R[12]=LOAD_INT(R[4]); R[4]+=4;
     C=(R[12]&1)==1; R[12]=(R[12]>>1)|(1<<31);
     if (C)
          goto _08015514;
     R[1]++;
     goto _080154A8;
_08015538:
     C=(R[6]&1)==1; R[1]=R[6]>>1;
     R[6]+=R[1];
     R[6]=R[5]-R[6];
     R[5]=LOAD_BYTE(R[6]-1);
     R[6]=LOAD_BYTE(R[6]-2);
     if (C)
     {
          R[1]=R[5]&0xF;
          R[1]=R[6]|(R[1]<<8);
     }
     if (!C)
     {
          R[1]=R[5]<<4;
          R[1]=R[1]|(R[6]>>4);
     }
     R[0]=R[1];
     return R[0];
}

signed int strings::NextChar_ext(unsigned int R[15], unsigned int input[8])
{
     bool C;
     if (Version()==0)
     {
          R[12]=LOAD_INT(FileTable()+0x10);
          R[12]=LOAD_INT(R[12]+0x56C);
     }
     else if (Version()==1)
     {
          R[12]=LOAD_INT(0x08680010);
          R[12]=LOAD_INT(R[12]+0x578);
     }
     else if (Version()==10)
     {
          R[12]=LOAD_INT(0x08800010);
          R[12]=LOAD_INT(R[12]+0x628);
     }
     else if (Version()==11)
     {
          R[12]=LOAD_INT(0x08C28010);
          R[12]=LOAD_INT(R[12]+0x4A4);
     }
     R[13]=0;
     R[4]=R[1]>>8;
     R[12]+=R[4]<<3;
     R[4]=LOAD_INT(R[12]); R[5]=LOAD_INT(R[12]+4);
     R[12]=R[1]&0xFF;
     R[12]+=R[12];
     R[5]=LOAD_SHORT(R[5]+R[12]);
     R[4]+=R[5];
     R[5]=R[4];
     R[12]=1;
     R[6]=R[4]&3;
     if (R[6]==0)
          goto _08015480;
     R[6]=(R[6]<<3)-R[12];
     R[4]=(R[4]|3)^3;
     R[12]=LOAD_INT(R[4]); R[4]+=4;
     R[12]=(R[12]>>1)|(1<<31);
     R[12]=R[12]>>R[6];
     R[6]=0;
_08015480:
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[4]); R[4]+=4;
          C=(R[12]&1)==1; R[12]=(R[12]>>1)|(1<<31);
     }
     if (C)
          goto _08015538;
     //if (R[6]>R[14])
     //{ return -R[13]; }
     C=(R[3]&1)==1; R[3]=R[3]>>1; R[13]++;
     if (!C)
          goto _08015480;
     if (R[3]==0)
     {
          R[3]=input[R[2]]; R[2]+=1;
          C=(R[3]&1)==1; R[3]=(R[3]>>1)|(1<<31);
     }
     if (!C)
          goto _08015480;
     R[1]=0;
_080154A8:
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (C)
          goto _08015510;
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (C)
          goto _080154D4;
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (C)
          goto _080154D0;
     C=(R[12]&1)==1; R[12]=R[12]>>1;
     if (C)
          goto _080154F0;
     R[1]+=4;
     goto _080154A8;
_080154D0:
     R[1]++;
_080154D4:
     if (R[12]!=0)
     {
          R[6]++;
          goto _080154A8;
     }
     R[12]=LOAD_INT(R[4]); R[4]+=4;
     C=(R[12]&1)==1; R[12]=(R[12]>>1)|(1<<31);
     if (!C)
          R[1]+=2;
     if (C)
          R[6]++;
     goto _080154A8;
_080154F0:
     R[1]+=2;
     goto _080154D4;
_08015510:
     if (R[12]==0)
          goto _08015524;
_08015514:
     R[6]++;
     R[1]--;
     if ((R[1]>>31)==0)
          goto _080154A8;
     goto _08015480;
_08015524:
     R[12]=LOAD_INT(R[4]); R[4]+=4;
     C=(R[12]&1)==1; R[12]=(R[12]>>1)|(1<<31);
     if (C)
          goto _08015514;
     R[1]++;
     goto _080154A8;
_08015538:
     if (R[6]!=R[14])
     {
          if (R[6]>R[14])
          {
               return -R[13];
          }
          else
          {
               return (R[13]|(1<<30));
          }
     }
     return R[13];
}

signed int strings::CompressStr(unsigned char *str, unsigned char *buffer, unsigned int pos)
{
     signed int num, nump;
     unsigned int i, ii, R[15], input[8], byte;
     unsigned short c, last, t, bits;
     i=0; last=0; byte=0; bits=0;
     do
     {
          if (Version()==0)
          {
               R[12]=LOAD_INT(FileTable()+0x10);
               R[12]=LOAD_INT(R[12]+0x56C);
          }
          else if (Version()==1)
          {
               R[12]=LOAD_INT(0x08680010);
               R[12]=LOAD_INT(R[12]+0x578);
          }
          else if (Version()==10)
          {
               R[12]=LOAD_INT(0x08800010);
               R[12]=LOAD_INT(R[12]+0x628);
          }
          else if (Version()==11)
          {
               R[12]=LOAD_INT(0x08C28010);
               R[12]=LOAD_INT(R[12]+0x4A4);
          }
          if (i!=0)
          { last=str[i-1]; }
          c=str[i]; ii=0;
          do
          {
               R[0]=ii>>1;
               
               R[1]=ii+R[0];
               
               R[0]=LOAD_SHORT(LOAD_INT(R[12]+4)+last*2);
               R[1]=R[0]+LOAD_INT(R[12])-R[1];
               
               R[0]=LOAD_BYTE(R[1]-1);
               R[1]=LOAD_BYTE(R[1]-2);
               if (ii&1)
               {
                    R[0]=R[0]&0xF;
                    R[0]=R[1]|(R[0]<<8);
               }
               else
               {
                    R[0]=R[0]<<4;
                    R[0]=R[0]|R[1]>>4;
               }
               ii++;
          }
          while (R[0]!=c);
          R[14]=ii-1;
          
          R[1]=last;
          R[2]=0;
          R[3]=1;
          input[0]=1;
          for (t=1; t<8; t++)
          { input[t]=0; }
          num=(strings::NextChar_ext(R,input));
          nump=0x0FFFFFFF;
          while (num<0 || (num>>30)==1)
          {
               if (num<0)
               {
                    num=-num;
                    while (((input[num>>5]>>(num&31))&1)!=1 && num>0)
                    { num--; }
                    input[num>>5]=input[num>>5]^(1<<(num&31)); num++;
                    input[num>>5]=input[num>>5]^(1<<(num&31)); num=-num;
               }
               else
               {
                    num=num&0x3FFFFFFF;
                    while (((input[num>>5]>>(num&31))&1)!=1 && num>0)
                    { num--; }
                    num++; input[num>>5]=input[num>>5]^(1<<(num&31)); num=num|(1<<30);
               }
               if (num==nump)
               { return -1; }
               nump=num;
               R[1]=last;
               R[2]=0;
               R[3]=1;
               num=(strings::NextChar_ext(R,input));
               //return input[0];
          }
          //return input[0];
          ii=0; R[13]=32;
          while (num>0)
          {
               //pos++; num=0; continue;
               if (R[13]==0)
               { ii++; R[13]=32; }
               if (num<R[13])
               { R[13]=num; }
               if (8-bits<=R[13])
               {
                    if (bits!=0)
                    { byte=byte|(((input[ii])&((1<<(8-bits))-1))<<bits); input[ii]=input[ii]>>(8-bits); R[13]-=(8-bits); num-=(8-bits); }
                    else
                    { byte=input[ii]&0xFF; input[ii]=input[ii]>>8; R[13]-=8; num-=8; }
                    buffer[pos]=byte&0xFF; pos++; byte=0; bits=0;
               }
               else
               {
                    input[ii]=input[ii]&((1<<R[13])-1);
                    if (bits!=0)
                    { byte=byte|(input[ii]<<bits); }
                    else
                    { byte=input[ii]; }
                    bits+=R[13];
                    input[ii]=0; num-=R[13]; R[13]=0;
               }
          }
          last=c; i++;
     }
     while (last!=0);
     if (bits!=0)
     { buffer[pos]=byte&0xFF; pos++; }
     return pos;
}
          
