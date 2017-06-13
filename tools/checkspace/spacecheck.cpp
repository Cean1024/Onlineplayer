#include "spacecheck.h"

spacecheck::spacecheck()
{

}

spacecheck::~spacecheck()
{

}
int spacecheck::getromspace()
{
    FILE * fd ;
    int ret;

    //std::string dealbuf;
    char buf[100];
    char *p = NULL;
    char * spit=" ";
    memset(buf,0,100);
    if( globaflags::getInstance()->getplatformflag() == platformisi386 )
        fd = popen("df | grep /dev/sda1","r");
    else
        fd = popen("df | grep /dev/root","r");

    if(fd < 0 ) {
        perror("popen");
        return FAILED;
    }
    ret = fread(buf,1,100,fd);
    //printf("popen read :%s \n",buf);
    pclose(fd);
    if(ret <= 0) {
        return FAILED;
    }
    p=strtok(buf,spit);
    p=strtok(NULL,spit);
    p=strtok(NULL,spit);
    p=strtok(NULL,spit);

    ret = atoi(p);
    return ret;
}

