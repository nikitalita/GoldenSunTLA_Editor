#include "compression.h"

int compression::compress(unsigned char *buffer, unsigned char *bufferi, unsigned int filesize)
{
    int rpos, rpos2, pos, bpos, bipos, tnum, tpos, length, mpos, tmpos, mlength, tmlength, maxdist, mpos2, tmpos2, maxpos2, num, num2, icnt, i;
    char byte;
    rpos=filesize-1; icnt=0; bpos=0;
    mpos = 0; mpos2 = 0;

    while (rpos>=0)
    {
          maxdist=0;
          mlength=0;
          do
          {
               rpos2=rpos;
               tmlength=mlength; tmpos=mpos; tmpos2=mpos2;
               mlength=0;
          do
          {
               num=bufferi[rpos2];
               bipos=rpos2-1;
               while ((bipos>(rpos2-4096)) && (bipos>mlength))
               {
                    num2=bufferi[bipos];
                    if (num2==num)
                    {
                         pos=bipos;
                         length=1;
                         while ((bufferi[bipos-length]==bufferi[rpos2-length]) && (length<4095))
                         {
                              length++;
                              if (((bipos-length)<0) || ((rpos2-length)<maxdist))
                              {
                                   break;
                              }
                         }
                         if (length>mlength)
                         {
                              mlength=length; mpos=bipos; mpos2=rpos2;
                         }
                    }
                    bipos-=1;
               }
               rpos2-=1;
          }
          while ((rpos2>(rpos-mlength)) && ((rpos2-maxdist)>mlength) && (mlength<272));
               maxdist=mpos2;
          }
          while ((maxdist<rpos-1) && (mlength>1) && (mlength<272));
          if (mlength<=1)
          { mlength=tmlength; mpos=tmpos; mpos2=tmpos2; }
          
          if ((mpos2<rpos) && (mlength>1))
          {
               while (rpos>mpos2)
               {
                    icnt++;
                    buffer[bpos]=bufferi[rpos]; bpos++;
                    if (icnt==255)
                    {
                        buffer[bpos]=icnt; bpos++; icnt=0;
                    }
                    rpos-=1;
                }
          }
          
          if (mlength>1)
          {
               if (icnt>0)
               { buffer[bpos]=icnt; bpos++; }
               buffer[bpos]=(mpos2-mpos)&0xFF; bpos++;
               buffer[bpos]=(((mpos2-mpos)>>8)&0xF)|(((mlength-1)&0xF)<<4); bpos++;
               buffer[bpos]=(((mlength-1)>>4)&0xFF);  bpos++;
               buffer[bpos]=0; bpos++;
               icnt=0;
          }
          else
          {
               icnt++;
               buffer[bpos]=bufferi[rpos]; bpos++;
               if (icnt==255)
               {
                    buffer[bpos]=icnt; bpos++; icnt=0;
               }
               mlength=1;
          }
          rpos-=mlength;
     }
     if (icnt>0)
     { buffer[bpos]=icnt; bpos++; icnt=0; }
     return bpos;
}

/*int compression::compress_bup(unsigned char *buffer, unsigned char *bufferi, unsigned int filesize)
{
    int rpos, rpos2, rpos3, pos, bpos, bipos, tnum, tpos, length, mpos, mlength, mpos2, maxpos2, num, num2, ipos, icnt;
    char byte;
    rpos=0; ipos=0; icnt=0; bpos=0;
    
    while (rpos<filesize)
    {
          rpos3=rpos;
          mlength=0;
          if ((rpos3-rpos<=mlength) && (rpos3<filesize-1))
          {
               rpos2=rpos3;
               while (rpos2<=rpos3+mlength)
               {
                    
               num=bufferi[rpos2];
               if (rpos2<4095)
               {
                    bipos=0;
               }
               else
               {
                    bipos=rpos2-4095;
               }
               while (bipos<rpos2)
               {
                    num2=bufferi[bipos]; bipos++;
                    if (num2==num)
                    {
                         pos=bipos-1;
                         length=1;
                         while ((bufferi[pos+length]==bufferi[rpos2+length]) && (length<4095))
                         {
                              length++;
                              if (rpos2+length>=filesize)
                              {
                                   break;
                              }
                         }
                         if (length>=mlength)
                         {
                              mlength=length; mpos=pos; mpos2=rpos2;
                         }
                    }
               }
               rpos2++;
               
               }
               
               if (mlength>1 && mpos2!=rpos3)
               {
                    mlength=0;
                    maxpos2=mpos2;
                    rpos2=rpos3;
                    num=bufferi[rpos2];
                    if (rpos2<4095)
                    {
                         bipos=0;
                    }
                    else
                    {
                         bipos=rpos2-4095;
                    }
                    while (bipos<rpos2)
                    {
                         num2=bufferi[bipos]; bipos++;
                         if (num2==num)
                         {
                              pos=bipos-1;
                              length=1;
                              while ((bufferi[pos+length]==bufferi[rpos2+length]) && (length<4095))
                              {
                                   length++;
                                   if (rpos2+length>=filesize)
                                   {
                                        break;
                                   }
                              }
                              if (length>=mlength)
                              {
                                   mlength=length; mpos=pos; mpos2=rpos2;
                              }
                         }
                    }
                    if (mlength+rpos2>maxpos2)
                    {
                         mlength=maxpos2-rpos2;
                    }
               }
               rpos2++;
               rpos3++;
          }
          if (mlength>1 && mpos2+1==rpos3)
          {
               while (rpos<mpos2)
               {
                    icnt++;
                    if ((icnt==1) || (icnt==256))
                    {
                         icnt=1; ipos=bpos; buffer[bpos]=1; bpos++;
                    }
                    else
                    {
                         buffer[ipos]=icnt;
                    }
                    buffer[bpos]=bufferi[rpos]; bpos++;
                    rpos++;
               }
               buffer[bpos]=0; bpos++;
               buffer[bpos]=(mpos2-mpos)&0xFF; bpos++;
               buffer[bpos]=(((mpos2-mpos)>>8)&0xF)|(((mlength-1)&0xF)<<4); bpos++;
               buffer[bpos]=(((mlength-1)>>4)&0xFF);  bpos++;
               icnt=0;
          }
          else
          {
               icnt++;
               if ((icnt==1) || (icnt==256))
               {
                    icnt=1; ipos=bpos; buffer[bpos]=1; bpos++;
               }
               else
               {
                    buffer[ipos]=icnt;
               }
               buffer[bpos]=bufferi[rpos]; bpos++;
               mlength=1;
          }
          rpos+=mlength;
     }
     return bpos;
}*/

int compression::recompress(unsigned char *buffer, unsigned char *buffero, unsigned int filesize, unsigned char format)
{
     /*void compress_byte();
     compress_byte=(compression::compress1_byte);
     compress_byte();*/
     return 0;
}

void compression::compress1_byte()
{
     return;
}

int compression::decompress(unsigned char *buffer, unsigned int pos, unsigned char format)
{
     unsigned int a, b, c, t, n, bits, bitnum, count, offset, opos, i, num, r, ppos, retlength;
     retlength=format>>7; format=format&15;
     ppos=pos;
     opos=0;
     a=0xFEDCBA98; b=0x76543210; c=15; t=0;
     if (format==0)
     {
          format=LOAD_BYTE(pos); pos++;
     }
     else
     {
          format--;
     }
     
     if (format>2) 
     {
          return -1;
     }
     
     if (format==0 || format==2)
     {
          bits=0;
          bitnum=0;
          if ((pos&1)==1)
          {
               bits=LOAD_BYTE(pos); pos++;
               bitnum=8;
          }
          
          bits|=(LOAD_SHORT(pos)<<bitnum); pos+=2;
          
          while (true)
          {
               count=0;
               if ((bits&1)==1)
               {
                    bits=(bits>>1); bitnum-=1;
                    if (format==0)
                    {
                         buffer[opos]=bits&255; opos++;
                         bits=(bits>>8); bitnum-=8;
                         if (((bitnum>>31)&1)==1)
                         {
                              bitnum+=16;
                              bits|=(LOAD_SHORT(pos)<<bitnum); pos+=2;
                         }
                    }
                    else
                    {
                         if ((bits&1)==1)
                         {
                              i=0;
                              bits=bits>>1; bitnum-=1;
                              num=bits&3;
                              bits=bits>>2; bitnum-=2;
                         }
                         else if ((bits&2)==2)
                         {
                              i=1;
                              bits=bits>>2; bitnum-=2;
                              num=bits&7;
                              bits=bits>>3; bitnum-=3;
                         }
                         else
                         {
                              i=2;
                              bits=bits>>2; bitnum-=2;
                              num=bits&15;
                              bits=bits>>4; bitnum-=4;
                         }
                         count=0; offset=0;
                         for (r=0; r<2; r++)
                         {
                              if (num==0)
                              {
                                   n=c&b;
                              }
                              else if (num<7)
                              {
                                   t=b>>((num+1)<<2);
                                   /*if (((8-num)<<2)>16)
                                   { t=t|(((b&0xFFFF)<<((8-num)<<2))&0xFFFFFFFF); }
                                   else
                                   { t=t|((b<<((8-num)<<2))&0xFFFFFFFF); }*/
                                   t=t|((b<<((8-num)<<2))&0xFFFFFFFF);
                                   n=c&(b>>(num<<2));
                                   b=n|(ror(t,(7-num)<<2));
                              }
                              else if (num==7)
                              {
                                   b=ror(b,28);
                                   n=b&c;
                              }
                              else if (num==8)
                              {
                                   n=a&c;
                                   t=a&0xFFFFFFF0;
                                   a=t|(b>>28);
                                   b=n|((b<<4)&0xFFFFFFFF);
                              }
                              else if (num<15)
                              {
                                   n=c&(a>>((num-8)<<2));
                                   t=a>>((num-7)<<2);
                                   t|=(a<<(((16-num)<<2)&0xFFFFFFFF));
                                   t=ror(t,(15-num)<<2);
                                   a=t|(b>>28);
                                   b=n|((b<<4)&0xFFFFFFFF);
                              }
                              else if (num==15)
                              {
                                   n=c&(a>>28);
                                   t=(a<<4)&0xFFFFFFFF;
                                   a=t|(b>>28);
                                   b=n|((b<<4)&0xFFFFFFFF);
                              }
                              count=count|(n<<offset); offset+=4;
                              switch (i)
                              {
                                   case 0: num=bits&3; bits=bits>>2; bitnum-=2; break;
                                   case 1: num=bits&7; bits=bits>>3; bitnum-=3; break;
                                   case 2: num=bits&15; bits=bits>>4; bitnum-=4; break;
                                   default: break;
                              }
                              i=0xFFFFFFFF;
                         }
                         if (((bitnum>>31)&1)==1)
                         {
                              bitnum+=16;
                              bits|=(LOAD_SHORT(pos)<<bitnum); pos+=2;
                         }
                         buffer[opos]=count; opos++;
                         count=0;
                    }
                    count=0;
               }
               else if ((bits&3)==0)
               {
                    count=2;
                    bits=(bits>>2); bitnum-=2;
               }
               else if ((bits&7)==2)
               {
                    count=3;
                    bits=(bits>>3); bitnum-=3;
               }
               else
               {
                    if ((bits&15)==6)
                    {
                         count=4;
                         bits=(bits>>4); bitnum-=4;
                    }
                    else if ((bits&31)==14)
                    {
                         count=5;
                         bits=(bits>>5); bitnum-=5;
                    }
                    else if ((bits&63)==62)
                    {
                         bits=(bits>>6); bitnum-=6;
                         count=(bits&3);
                         bits=(bits>>2); bitnum-=2;
                         if (count==0)
                         {
                              count=(bits&127);
                              bits=(bits>>7); bitnum-=7;
                              if (count==0)
                              {
                                   if (retlength==1)
                                   {
                                        if (bitnum==0)
                                        { pos-=2; bitnum=0; }
                                        else if (((bitnum>>31)&1)==1)
                                        { bitnum+=16; }
                                        return (pos-ppos-((bitnum)>>3));//*32+bitnum;//((bitnum+16)>>3);
                                   }
                                   else
                                   { return opos; }
                              }
                              else
                              {
                                   count+=10;
                              }
                         }
                         else
                         {
                              count+=7;
                         }
                    }
                    else if ((bits&127)==30)
                    {
                         count=6;
                         bits=(bits>>7); bitnum-=7;
                    }
                    else
                    {
                         count=7;
                         bits=(bits>>7); bitnum-=7;
                    }
                    if (((bitnum>>31)&1)==1)
                    {
                         bitnum+=16;
                         bits|=(LOAD_SHORT(pos)<<bitnum); pos+=2;
                    }
               }
               if (count!=0)
               {
                    if ((bits&1)==0)
                    {
                         bits=(bits>>1); bitnum-=1;
                         offset=opos-33;
                         if (offset<2048)
                         {
                              num=11;
                              offset=(offset<<21);
                              r=((offset>>31)&1);
                              offset=(offset<<1);
                              while (r==0 && num>0)
                              {
                                   num-=1;
                                   r=((offset>>31)&1);
                                   offset=(offset<<1);
                              }
                              offset=(1<<num)-1;
                              offset=(bits&offset);
                              offset+=32;
                              bits=(bits>>num); bitnum-=num;
                         }
                         else
                         {
                              offset=(bits&4095);
                              bits=(bits>>12); bitnum-=12;
                              offset+=32;
                         }
                    }
                    else
                    {
                         bits=(bits>>1); bitnum-=1;
                         offset=(bits&31);
                         bits=(bits>>5); bitnum-=5;
                    }
                    if (((bitnum>>31)&1)==1)
                    {
                         bitnum+=16;
                         bits|=(LOAD_SHORT(pos)<<bitnum); pos+=2;
                    }
                    if (opos<offset)
                    {
                         return 0;
                    }
                    for (r=0; r<count; r++)
                    {
                         buffer[opos]=buffer[opos-offset-1]; opos++;
                    }
               }
          }
     }
     else
     {
          bits=0;
          while (true)
          {
               if (bits==0)
               {
                    bits=(LOAD_BYTE(pos)<<24)|(1<<23); pos++;
               }
               c=(bits>>31);
               bits=(bits<<1);
               while (c!=1)
               {
                    buffer[opos]=LOAD_BYTE(pos); opos++; pos++;
                    c=(bits>>31);
                    bits=(bits<<1);
               }
               if (bits!=0)
               {
                    count=LOAD_BYTE(pos); pos++;
                    num=LOAD_BYTE(pos); pos++;
                    offset=count&240;
                    offset=num|(offset<<4);
                    count&=15;
                    if (count==0)
                    {
                         if (offset==0)
                         {
                              if (retlength==1)
                              {
                                   //bitnum=0; bits=bits<<1;
                                   //while (bits!=0)
                                   //{ bitnum++; bits=bits<<1; }
                                   return pos-ppos;//-((bitnum+8)>>3);
                              }
                              else
                              { return opos; }
                         }
                         count=LOAD_BYTE(pos)+16; pos++;
                    }
                    if (opos<offset)
                    {
                         return 0;
                    }
                    for (r=0; r<count+1; r++)
                    {
                         buffer[opos]=buffer[opos-offset]; opos++;
                    }
               }
          }
     }
     return -2;
}


int compression::decompress16(unsigned char *buffer, unsigned int pos)
{
     unsigned int R[15], c;
     R[0]=pos;
     R[1]=0;
     R[2]=0xFEDCBA98;
     R[3]=0x76543210;
     R[5]=0xF;
     R[12]=0x1;
     R[6]=R[0]&3; //08038b2C	ands r6, r0, #0x03
     R[14]=0;
     if (R[6]==0)
     { //08038b30	beq $08038B70
          goto _08038B70;
     }
     R[6]=(R[6]<<3)-R[12]; //08038b34	rsbs r6, r12, r6, lsl #0x03
     R[0]=(R[0]|3)^3; //08038b38	bic r0, r0, #0x3
     R[12]=LOAD_INT(R[0]); R[0]+=4; //08038b3c	ldr r12, [r0], #0x4
     R[12]=(R[12]>>1)|(1<<31); //08038b40	mov r12, r12, rrx
     R[12]=R[12]>>R[6]; //08038b44	mov r12, r12, lsr r6
     goto _08038B70; //08038b48	b $08038B70
     
_08038B54:
     R[2]=R[4]|(R[3]>>0x1C); //08038b54	orr r2, r4, r2, lsr #0x1c
     R[3]=R[6]|(R[3]<<4); //08038b58	orr r3, r6, r3, lsl #0x04
_08038B5C:
     R[6]=R[3]&R[5]; //08038b5c	and r6, r3, r5
_08038B60:
     buffer[R[1]]=R[6]; R[1]++; //08038b60	strb r6, [r1], #0x1
     c=R[12]&1; R[12]=R[12]>>1; //08038b64	movs r12, r12, lsr #0x01
     if (c==1)
     { //08038b68	bcs $08038B78
          goto _08038B78;
     }
     buffer[R[1]]=R[6]; R[1]++; //08038B6C strb r6, [r1], #0x1
_08038B70:
     c=R[12]&1; R[12]=R[12]>>1; //08038b70	movs r12, r12, lsr #0x01
     if (c==0)
     { //08038b74	bcc 08038B5C
          goto _08038B5C;
     }
_08038B78:
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038b78	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038b7c	moveqs r12, r12, rrx
     }
     if (c==0)
     { //08038b80	bcc #08038B5C
          goto _08038B5C;
     }
     c=R[12]&1; R[12]=R[12]>>1; //08038b84	movs r12, 12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038b88	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038b8c	moveqs r12, r12, rrx
     }
     if (c==0)
     { //08038b90	bcc $08038D2C
          goto _08038D2C;
     }
     c=R[12]&1; R[12]=R[12]>>1; //08038b94	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038b98	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038b9c	moveqs r12, r12, rrx
     }
     if (c==0)
     { //08038bA0	bcc $08038CF4
          goto _08038CF4;
     }
     c=R[12]&1; R[12]=R[12]>>1; //08038bA4	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038bA8	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038bAc	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038bB0	bcs $08038BC8
          goto _08038BC8;
     }
     R[4]=R[3]>>0x1C; //08038bB4	mov r4, r3, lsr #0x1C
     R[4]=R[4]|(R[3]<<8); //08038bB8	orr r4, r4, r3, lsl #0x08
     R[6]=R[5]&(R[3]>>0x18); //08038bBc	and r6, r5, r3, lsr #0x18
     R[3]=R[6]|ror(R[4],4); //08038bC0	orr r3, r6, r4, ror #0x04
     R[14]|=1;
     goto _08038B60; //08038bC4	b $08038B60
_08038BC8:
     c=R[12]&1; R[12]=R[12]>>1; //08038bC8	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038bCc	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038bD0	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038bD4	bcs $08038C00
          goto _08038C00;
     }
     c=R[12]&1; R[12]=R[12]>>1; //08038bD8	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038bDc	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038bE0	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038bE4	bcs $08038BF4
          goto _08038BF4;
     }
     R[3]=ror(R[3],0x1C); //08038bE8	mov r3, r3, ror #0x1C
     R[6]=R[5]&R[3]; //08038bEc	and r6, r5, r3
     R[14]|=2;
     goto _08038B60; //08038bF0	b $08038B60
_08038BF4:
     R[6]=R[5]&R[2]; //08038bF4	and r6, r5, r2
     R[4]=R[2]^(R[2]&0xF); //08038bF8	bic r4, r2, #0xF
     R[14]|=4;
     goto _08038B54; //08038bFc	b $08038B54
_08038C00:
     c=R[12]&1; R[12]=R[12]>>1; //08038C00	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038C04	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038C08	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038C0c	bcs $08038C24
          goto _08038C24;
     }
     R[6]=R[5]&(R[2]>>4); //08038C10	and r6, r5, r2, lsr #0x04
     R[4]=R[2]>>8; //08038C14	mov r4, r2, lsr #0x08
     R[4]=R[4]|(R[2]<<0x1C); //08038C18	orr r4, r4, r2, lsl #0x1C
     R[4]=ror(R[4],0x18); //08038C1c	mov r4, r4, ror #0x18
     R[14]|=8;
     goto _08038B54; //08038C20	b $08038B54
_08038C24:
     c=R[12]&1; R[12]=R[12]>>1; //08038C24	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038C28	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038C2c	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038C30	bcs $08038C6C
          goto _08038C6C;
     }
     c=R[12]&1; R[12]=R[12]>>1; //08038C34	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038C38	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038C3c	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038C40	bcs $08038C58
          goto _08038C58;
     }
     R[6]=R[5]&(R[2]>>8); //08038C44	and r6, r5, r2, lsr #0x08
     R[4]=R[2]>>0xC; //08038C48	mov r4, r2, lsr #0x0C
     R[4]=R[4]|(R[2]<<0x18); //08038C4c	orr r4, r4, r2, lsl #0x18
     R[4]=ror(R[4],0x14); //08038C50	mov r4, r4, ror #0x14
     R[14]|=16;
     goto _08038B54; //08038C54	b #08038B54
_08038C58:
     R[6]=R[5]&(R[2]>>0xC); //08038C58	and r6, r5, r2, lsr #0x0C
     R[4]=R[2]>>0x10; //08038C5c	mov r4, r2, lsr #0x10
     R[4]=R[4]|(R[2]<<0x14); //08038C60	orr r4, r4, r2, lsl #0x14
     R[4]=ror(R[4],0x10); //08038C64	mov r4, r4, ror #0x10
     R[14]|=32;
     goto _08038B54; //08038C68	b #08038B54
_08038C6C:
     c=R[12]&1; R[12]=R[12]>>1; //08038C6c	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038C70	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038C74	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038C78	bcs #08038C90
          goto _08038C90;
     }
     R[6]=R[5]&(R[2]>>0x10); //08038C7c	and r6, r5, r2, lsr #0x10
     R[4]=R[2]>>0x14; //08038C80	mov r4, r2, lsr #0x14
     R[4]=R[4]|(R[2]<<0x10); //08038C84	orr r4, r4, r2, lsl #0x10
     R[4]=ror(R[4],0xC); //08038C88	mov r4, r4, ror #0x0C
     R[14]|=64;
     goto _08038B54; //08038C8c	b #08038B54
_08038C90:
     c=R[12]&1; R[12]=R[12]>>1; //08038C90	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038C94	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038C98	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038C9c	bcs #08038CD8
          goto _08038CD8;
     }
     c=R[12]&1; R[12]=R[12]>>1; //08038CA0	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038CA4	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038CA8	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038CAc	bcs #08038CC4
          goto _08038CC4;
     }
     R[6]=R[5]&(R[2]>>0x14); //08038CB0	and r6, r5, r2, lsr #0x14
     R[4]=R[2]>>0x18; //08038CB4	mov r4, r2, lsr #0x18
     R[4]=R[4]|(R[2]<<0xC); //08038CB8	orr r4, r4, r2, lsl #0x0C
     R[4]=ror(R[4],8); //08038CBc	mov r4, r4, ror #0x08
     R[14]|=128;
     goto _08038B54; //08038CC0	b #08038B54
_08038CC4:
     R[6]=R[5]&(R[2]>>0x18); //08038CC4	and r6, r5, r2, lsr #0x18
     R[4]=R[2]>>0x1C; //08038CC8	mov r4, r2, lsr #0x1C
     R[4]=R[4]|(R[2]<<8); //08038CCc	orr r4, r4, r2, lsl #0x08
     R[4]=ror(R[4],4); //08038CD0	mov r4, r4, ror #0x04
     R[14]|=256;
     goto _08038B54; //08038CD4	b #08038B54
_08038CD8:
     c=R[12]&1; R[12]=R[12]>>1; //08038CD8	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038CDc	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038CE0	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038CE4	bcs $08038D88
          goto _08038D88;
     }
     R[6]=R[5]&(R[2]>>0x1C); //08038CE8	and r6, r5, r2, lsr #0x1C
     R[4]=R[2]<<4; //08038CEc	mov r4, r2, lsl #0x04
     R[14]|=512;
     goto _08038B54; //08038CF0	b #08038B54
_08038CF4:
     c=R[12]&1; R[12]=R[12]>>1; //08038CF4	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038CF8	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038CFc	moveqs r12, r12, rrx
     }
     if (c==0)
     { //08038D00	bcc #08038D18
          goto _08038D18;
     }
     R[4]=R[3]>>0x18; //08038D04	mov r4, r3, lsr #0x18
     R[4]=R[4]|(R[3]<<0xC); //08038D08	orr r4, r4, r3, lsl #0x0C
     R[6]=R[5]&(R[3]>>0x14); //08038D0c	and r6, r5, r3, lsr #0x14
     R[3]=R[6]|ror(R[4],8); //08038D10	orr r3, r6, r4, ror #0x08
     R[14]|=1024;
     goto _08038B60; //08038D14	b #08038B60
_08038D18:
     R[4]=R[3]>>0x14; //08038D18	mov r4, r3, lsr #0x14
     R[4]=R[4]|(R[3]<<0x10); //08038D1c	orr r4, r4, r3, lsl #0x10
     R[6]=R[5]&(R[3]>>0x10); //08038D20	and r6, r5, r3, lsr #0x10
     R[3]=R[6]|ror(R[4],0xC); //08038D24	orr r3, r6, r4, ror #0x0C
     R[14]|=2048;
     goto _08038B60; //08038D28	b #08038B60
_08038D2C:
     c=R[12]&1; R[12]=R[12]>>1; //08038D2c	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038D30	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038D34	moveqs r12, r12, rrx
     }
     if (c==0)
     { //08038D38	bcc #08038D74
          goto _08038D74;
     }
     c=R[12]&1; R[12]=R[12]>>1; //08038D3c	movs r12, r12, lsr #0x01
     if (R[12]==0)
     {
          R[12]=LOAD_INT(R[0]); R[0]+=4; //08038D40	ldreq r12, [r0], #0x4
          c=R[12]&1; R[12]=(R[12]>>1)|(1<<31); //08038D44	moveqs r12, r12, rrx
     }
     if (c==1)
     { //08038D48	bcs #08038D60
          goto _08038D60;
     }
     R[4]=R[3]>>0xC; //08038D4c	mov r4, r3, lsr #0x0C
     R[4]=R[4]|(R[3]<<0x18); //08038D50	orr r4, r4, r3, lsl #0x18
     R[6]=R[5]&(R[3]>>8); //08038D54	and r6, r5, r3, lsr #0x08
     R[3]=R[6]|ror(R[4],0x14); //08038D58	orr r3, r6, r4, ror #0x14
     R[14]|=4096;
     goto _08038B60; //08038D5c	b #08038B60
_08038D60:
     R[4]=R[3]>>0x10; //08038D60	mov r4, r3, lsr #0x10
     R[4]=R[4]|(R[3]<<0x14); //08038D64	orr r4, r4, r3, lsl #0x14
     R[6]=R[5]&(R[3]>>0xC); //08038D68	and r6, r5, r3, lsr #0x0C
     R[3]=R[6]|ror(R[4],0x10); //08038D6c	orr r3, r6, r4, ror #0x10
     R[14]|=8192;
     goto _08038B60; //08038D70	b #08038B60
_08038D74:
     R[4]=R[3]>>8; //08038D74	mov r4, r3, lsr #0x08
     R[4]=R[4]|(R[3]<<0x1C); //08038D78	orr r4, r4, r3, lsl #0x1C
     R[6]=R[5]&(R[3]>>4); //08038D7c	and r6, r5, r3, lsr #0x04
     R[3]=R[6]|ror(R[4],0x18); //08038D80	orr r3, r6, r4, ror #0x18
     R[14]|=16384;
     goto _08038B60; //08038D84	b #08038B60
_08038D88:
     //08038D88	ldmfd sp!, {r5, r6}
     return R[1]; //08038D8c	bx lr
}
