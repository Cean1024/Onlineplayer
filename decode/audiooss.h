#ifndef AUDIOOSS_H
#define AUDIOOSS_H

#include "thread.h"
#include "globaflags.h"
#ifdef CONFIG_AUDIOOSS


#define DSP "/dev/dsp"
#define MIXER "/dev/mixer"
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <fcntl.h>

class audiooss:public Thread
{
private:
    int oss_fd;
    int status;

public:
    static audiooss &getInstance(){
     static audiooss Audiooss;
     return Audiooss;
    }
    audiooss();
    ~audiooss();
    int init();

protected:
    void run();
    int audioinit();

};
#endif

#endif // AUDIOOSS_H
