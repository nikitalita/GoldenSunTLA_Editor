#include "spacemanager.h"

int spacemanager::list_num;

int spacemanager::organizeList(int *list)
{
     int i, ii, pos, val, size;
     for (i=0; i<list_num-1; i++)
     {
          val=list[(i<<1)+1]; pos=i;
          for (ii=i+1; ii<list_num; ii++)
          {
               if (list[(ii<<1)+1]<val)
               {
                    pos=ii; val=list[(ii<<1)+1];
               }
          }
          if (pos!=i)
          {
               val=list[pos<<1];
               size=list[(pos<<1)+1];
               list[pos<<1]=list[i<<1];
               list[(pos<<1)+1]=list[(i<<1)+1];
               if (val==list[i<<1])
               { list[i<<1]=val|0x40000000; }
               else
               { list[i<<1]=val; }
               list[(i<<1)+1]=size;
          }
          if (list[i<<1]==list[(i-1)<<1])
          {
               list_num--;
               for (ii=i; ii<list_num; ii++)
               {
                    list[ii<<1]=list[(ii+1)<<1];
                    list[(ii<<1)+1]=list[((ii+1)<<1)+1];
               }
               list[list_num<<1]=0; list[(list_num<<1)+1]=0;
               i--;
          }
     }
     //return list_num;
     if ((LOAD_INT(FileTable())>>24)!=8)
     { return list_num; }
     if ((LOAD_INT(FileTable())&0x1FFFFFF)!=0)
     {
          int tlist[32768]; int tnum, tnum2; tnum=list_num;
          pos=LOAD_INT(FileTable());
          WRITE_INT(FileTable(), pos|0x70000000);
          tnum2=spacemanager::mapSpace(tlist); list_num=tnum;
          spacemanager::freeSpace(list, pos,(tnum2<<3)+4);
     }
     else
     { WRITE_INT(FileTable(), 0x7FFFFFFF); }
     pos=spacemanager::findSpace(list, (list_num<<3)+4);
     if (pos>0)
     {
          spacemanager::claimSpace(list, pos, (list_num<<3)+4);
          i=0;
          for (i=0; i<list_num; i++)
          {
               val=list[i<<1];
               WRITE_INT(pos+(i<<3),list[i<<1]);
               WRITE_INT(pos+(i<<3)+4,list[(i<<1)+1]);
          }
          WRITE_INT(pos+(list_num<<3),0x7FFFFFFF);
          WRITE_INT((FileTable()),pos);
          //WRITE_INT(0,0x12345678);
     }
     else
     {
          WRITE_INT(FileTable(),0x08000000);
     }
     return list_num;
}

int spacemanager::mapSpace(int *list)
{
     int pos, val, size;
     for (list_num=0; list_num<32768; list_num++)
     { list[list_num]=0; }
     list_num=0;
     pos=LOAD_INT(FileTable())&0x09FFFFFF;
     if ((pos&0x1FFFFFF)!=0x0)
     {
          val=LOAD_INT(pos+(list_num<<3));
          while (val!=0x7FFFFFFF)
          {
               list[list_num<<1]=val;
               list[(list_num<<1)+1]=LOAD_INT(pos+4+(list_num<<3)); list_num++;
               val=LOAD_INT(pos+(list_num<<3));
               if (val==0)
               { val=0x7FFFFFFF; }
          }
          list[list_num<<1]=0;
          list[(list_num<<1)+1]=0;
          return list_num;
     }
     val=0;
     do
     {
          if (val==1)
          { val++; continue; }
          pos=LOAD_INT(FileTable()+0x4+(val<<2))&0x1FFFFFF;
          if (pos>(FileTable()&0x1FFFFFF))
          {
               pos=FileSize();
          }
          size=0;
          do
          { size++; }
          while (LOAD_BYTE(pos-size)==0);
          size--;
          size=size&0xFFFFFFFC;
          pos-=size;
          if (pos+size==FileSize())
          {
               size+=(32<<20)-FileSize();
          }
          if (size>0)
          {
               list[list_num<<1]=pos|0x08000000;
               list[(list_num<<1)+1]=size;
               list_num++;
          }
          val++;
     }
     while (pos<=(FileTable()&0x1FFFFFF));
     spacemanager::organizeList(list);
     return list_num;
}

int spacemanager::findSpace(int*list, int size)
{
     //size=(size+3)&0xFFFFFFFC;
     int pos, i;
     do
     {
          pos=0;
          for (i=0; i<list_num; i++)
          {
               if (list[(i<<1)+1]>=size)
               {
                    if (spacemanager::confirmSpace(list,list[i<<1],int(size)))
                    {
                         pos=list[i<<1]; i=list_num;
                    }
                    else
                    {
                         pos=-1; i=list_num;
                    }
               }
          }
     }
     while (pos==-1);
     if (pos==0)
     { return pos; }
     return (pos&0x1FFFFFF)|0x08000000;
}
     
int spacemanager::confirmSpace(int *list, int pos, int size)
{
     pos=(pos+3)&0xFFFFFFFC;
     //size=(size+3)&0xFFFFFFFC;
     int i, index; index=-1;
     for (i=0; i<list_num; i++)
     {
          if ((list[i<<1]&0x1FFFFFF)<=(pos&0x1FFFFFF))
          {
               if (((list[i<<1]+list[(i<<1)+1])&0x1FFFFFF)>=((pos+size)&0x1FFFFFF))
               {
                    index=i; i=list_num;
               }
          }
     }
     if (index<0)
     { return -list_num; }
     if ((list[index<<1]>>28)==1)
     { return list_num; }
     int length, ppos; ppos=pos;
     pos=list[index<<1];
     for (length=0; length<size+(ppos-list[index<<1]); length++)
     {
          if  (LOAD_BYTE(pos+length)!=0)
          {
               size=list[(index<<1)+1]; length--;
               list[(index<<1)+1]=length;
               pos+=size; length=0;
               do
               {
                    length++;
               }
               while (LOAD_BYTE(pos-length)==0);
               length--;
               pos-=length;
               list[list_num<<1]=pos|0x08000000;
               list[(list_num<<1)+1]=length;
               list_num++;
               spacemanager::organizeList(list);
               return -list_num;
          }
     }
     return list_num;
}

int spacemanager::freeSpace(int *list, int pos, int size)
{
     int i, ii, index, pos2, pos3; index=-1; pos2=pos+size;
     pos|=0x10000000;
     for (i=0; i<list_num; i++)
     {
          if ((list[i<<1]&0x1FFFFFF)>(pos&0x1FFFFFF))
          {
               if ((pos2&0x1FFFFFF)>=(list[i<<1]&0x1FFFFFF))
               {
                    pos=pos&0x09FFFFFF|(list[i<<1]&0xF6000000);
                    pos3=list[i<<1]+list[(i<<1)+1];
                    if ((pos3&0x1FFFFFF)>(pos2&0x1FFFFFF))
                    {
                        pos2=pos3;
                        size=(pos2&0x09FFFFFF)-(pos&0x09FFFFFF);
                    }
                    list_num-=1;
                    for (ii=i; ii<list_num; ii++)
                    {
                        list[ii<<1]=list[(ii+1)<<1];
                        list[(ii<<1)+1]=list[((ii+1)<<1)+1];
                    }
                    list[list_num<<1]=0;
                    list[(list_num<<1)+1]=0;
                    i=-1;
               }
          }
          else
          {
               pos3=list[i<<1]+list[(i<<1)+1];
               if ((pos&0x1FFFFFF)<=(pos3&0x1FFFFFF))
               {
                    pos=list[i<<1];
                    if ((pos3&0x1FFFFFF)>(pos2&0x1FFFFFF))
                    {
                        pos2=pos3;
                    }
                    size=(pos2&0x09FFFFFF)-(pos&0x09FFFFFF);
                    list_num-=1;
                    for (ii=i; ii<list_num; ii++)
                    {
                        list[ii<<1]=list[(ii+1)<<1];
                        list[(ii<<1)+1]=list[((ii+1)<<1)+1];
                    }
                    list[list_num<<1]=0;
                    list[(list_num<<1)+1]=0;
                    i=-1;
               }
          }
     }
     
     list[list_num<<1]=(pos&0x1FFFFFF)|0x08000000;
     if ((pos>>28)==1)
     { list[list_num<<1]|=0x10000000; }
     else
     {
          for (i=0; i<size; i++)
          { WRITE_BYTE(pos+i,0); }
     }
     list[(list_num<<1)+1]=size;
     list_num++;
     spacemanager::organizeList(list);
     return list_num;
}

int spacemanager::claimSpace(int *list, int pos, int size)
{
     pos=(pos+3)&0xFFFFFFFC;
     //size=(size+3)&0xFFFFFFFC;
     if (pos<=0 || size<=0)
     { return -list_num; }
     if (spacemanager::confirmSpace(list, pos, size)<=0)
     { return -list_num; }
     int i, index; index=-1;
     for (i=0; i<list_num; i++)
     {
          if ((list[i<<1]&0x1FFFFFF)<=(pos&0x1FFFFFF))
          {
               if (((list[i<<1]+list[(i<<1)+1])&0x1FFFFFF)>=((pos+size)&0x1FFFFFF))
               {
                    index=i; i=list_num;
               }
          }
     }
     if (index<0)
     { return -list_num; }
     if ((pos&0x1FFFFFF)==(list[index<<1]&0x1FFFFFF))
     {
          if (size<list[(index<<1)+1])
          {
               list[index<<1]+=size;
               list[(index<<1)+1]-=size;
               spacemanager::organizeList(list);
               return list_num;
          }
          else
          {
               list_num-=1;
               for (i=index; i<list_num; i++)
               {
                    list[i<<1]=list[(i+1)<<1];
                    list[(i<<1)+1]=list[((i+1)<<1)+1];
               }
               list[list_num<<1]=0; list[(list_num<<1)+1]=0;
               spacemanager::organizeList(list);
               return list_num;
          }
     }
     else
     {
          i=list[(index<<1)+1]-((pos&0x1FFFFFF)-(list[index<<1]&0x1FFFFFF))-size;
          if (i>0)
          {
               list[list_num<<1]=((pos+size)&0x1FFFFFF)|0x08000000;
               list[(list_num<<1)+1]=i;
               list_num+=1;
          }
          list[(index<<1)+1]=(pos&0x1FFFFFF)-(list[index<<1]&0x1FFFFFF);
     }
     spacemanager::organizeList(list);
     return list_num;
}
