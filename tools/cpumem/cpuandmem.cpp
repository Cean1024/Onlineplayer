#include "cpuandmem.h"

cpuandmem::cpuandmem()
{

}

cpuandmem::~cpuandmem()
{

}

uint cpuandmem::getfreemem(MEM_OCCUPY &mem)
{
    return get_memoccupy(&mem);
}

int cpuandmem::get_memoccupy (MEM_OCCUPY *mem) //对无类型get函数含有一个形参结构体类弄的指针O
 {
     FILE *fd;
     int n;
     char buff[256];
     MEM_OCCUPY *m;
     m=mem;

     fd = fopen ("/proc/meminfo", "r");
     if(fd < 0)
         return FAILED;

     fgets (buff, sizeof(buff), fd);
     sscanf (buff, "%s %u %s", m->name, &m->total , m->name2);

     fgets (buff, sizeof(buff), fd); //从fd文件中读取长度为buff的字符串再存到起始地址为buff这个空间里
     sscanf (buff, "%s %u", m->name2, &m->free);

     fclose(fd);     //关闭文件fd
     return SUCCESS;
 }
int cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n)
 {
     unsigned long od, nd;
     unsigned long id, sd;
     int cpu_use = 0;

     od = (unsigned long) (o->user + o->nice + o->system +o->idle);//第一次(用户+优先级+系统+空闲)的时间再赋给od
     nd = (unsigned long) (n->user + n->nice + n->system +n->idle);//第二次(用户+优先级+系统+空闲)的时间再赋给od

     id = (unsigned long) (n->user - o->user);    //用户第一次和第二次的时间之差再赋给id
     sd = (unsigned long) (n->system - o->system);//系统第一次和第二次的时间之差再赋给sd
     if((nd-od) != 0)
        cpu_use = (int)((sd+id)*100)/(nd-od); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
     else
        cpu_use = FAILED;
     //printf("cpu: %u/n",cpu_use);
     return cpu_use;
 }



int get_cpuoccupy (CPU_OCCUPY *cpust) //对无类型get函数含有一个形参结构体类弄的指针O
 {
     FILE *fd;
     int n;
     char buff[256];
     CPU_OCCUPY *cpu_occupy;
     cpu_occupy=cpust;

     fd = fopen ("/proc/stat", "r");
     if(fd < 0)
        return FAILED;
     fgets (buff, sizeof(buff), fd);

     sscanf (buff, "%s %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle);

     fclose(fd);
     return SUCCESS;
 }

int cpuandmem::getcpuusedrate()
 {
     CPU_OCCUPY cpu_stat1;
     CPU_OCCUPY cpu_stat2;
     //MEM_OCCUPY mem_stat;
     int cpu;

     //获取内存
     //get_memoccupy ((MEM_OCCUPY *)&mem_stat);

     //第一次获取cpu使用情况
     get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1);
     sleep(1);

     //第二次获取cpu使用情况
     get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2);

     //计算cpu使用率
     cpu = cal_cpuoccupy ( (CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2);

     return cpu;
 }
