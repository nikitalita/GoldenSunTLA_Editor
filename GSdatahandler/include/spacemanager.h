#include "includes.h"

class spacemanager
{
public:
     static int list_num;
     static int organizeList(int *list);
     static int mapSpace(int *list);
     static int findSpace(int *list, int size);
     static int confirmSpace(int *list, int index, int size);
     static int freeSpace(int *list, int pos, int size);
     static int claimSpace( int *list, int pos, int size);
};
