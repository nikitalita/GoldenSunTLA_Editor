#ifndef INCLUDES_H
#define INCLUDES_H
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
//#include <string>
#include <stdio.h>

using namespace std;

#define EXPORTREAL extern "C" __declspec(dllexport) double __cdecl
#define EXPORTSTRING extern "C" __declspec(dllexport) LPSTR __cdecl
//#define EXPORTSTRING extern "C" __declspec(dllexport) str* __cdecl

EXPORTREAL loadFile(char* name);

#include "dllmain.h"
#endif // INCLUDES_H