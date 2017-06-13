#include "audiooss.h"

#ifdef CONFIG_AUDIOOSS
audiooss::audiooss()
{

}

audiooss::~audiooss()
{

}

int audiooss::audioinit()
{
    oss_fd=open(DSP,O_RDWR);
    if ( oss_fd < 0) {
        perror("OPEN");
        return FFAILD;
    }
}

int audiooss::init()
{
    int ret;
    ret = audioinit();
    if( ret < 0)
        return IFAILED;
    start();
    return ISUCCESS;
}

void audiooss::run()
{
    int count=100;
    while(count--)
    {
        printf("audio runing \n");
        sleep(1);
        if( count < 90 ){
            //getState();
            distroy();
            //join();
        }
    }
}
#endif
