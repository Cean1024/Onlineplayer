#ifndef CPUANDMEM_H
#define CPUANDMEM_H
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globaflags.h"

typedef struct PACKEDMEM       //定义一个cpu occupy的结构体
{
char name[20];      //定义一个char类型的数组名name有20个元素
unsigned int user; //定义一个无符号的int类型的user
unsigned int nice; //定义一个无符号的int类型的nice
unsigned int system;//定义一个无符号的int类型的system
unsigned int idle; //定义一个无符号的int类型的idle
}CPU_OCCUPY;

typedef struct PACKEDCPU       //定义一个mem occupy的结构体
{
char name[20];      //定义一个char类型的数组名name有20个元素
unsigned long total;
char name2[20];
unsigned long free;
}MEM_OCCUPY;



class cpuandmem
{
public:
    cpuandmem();
    ~cpuandmem();
    uint getfreemem(MEM_OCCUPY &mem);
    int getcpuusedrate();
    int get_memoccupy (MEM_OCCUPY *mem);
};

#endif // CPUANDMEM_H
