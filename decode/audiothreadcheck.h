#ifndef AUDIOTHREADCHECK_H
#define AUDIOTHREADCHECK_H
#include "globaflags.h"
//#include "decode/audio.h"

class audiothreadcheck:public Thread
{
public:
    audiothreadcheck();
    ~audiothreadcheck();

protected:
    void run();

};

#endif // AUDIOTHREADCHECK_H
