/*
* This file is part of the M2Plus - Nomad Group
*
* See LICENSE in the root of the source tree for information
* regarding licensing.
*/

#pragma comment(linker, "/merge:.data=.cld")
#pragma comment(linker, "/merge:.rdata=.clr")
#pragma comment(linker, "/merge:.cl=.zdata")
#pragma comment(linker, "/merge:.text=.zdata")
#pragma comment(linker, "/section:.zdata,re")

// game
#pragma bss_seg(".maf")
char maf_seg[0x300000];

// alignment
#pragma data_seg(".zdata")
char zdata[200] = { 1 };
