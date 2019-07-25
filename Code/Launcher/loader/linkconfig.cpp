#pragma comment(linker, "/merge:.text=.zdata")

// game
#pragma bss_seg(".mafia1")
char maf_seg[0x400000];
