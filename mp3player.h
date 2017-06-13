#ifndef MP3PLAYER_H
#define MP3PLAYER_H
#include <iostream>
#include <signal.h>
#include "include/mad/mad.h"
#include "globaflags.h"
#include "decode/buffercache/cachebuf.h"
#include "decode/mp3/mp3decode.h"
#include "tools/checkspace/spacecheck.h"
#include "tools/times/gettime.h"

typedef  std::string (* musiccallback)(void);


class mp3player:public Thread
{
public:
    mp3player();
    ~mp3player();
    int mp3play(std::string &music , int card, sem_t &semofnext);
    void mp3stop(int card);
    int checkspace();
    r_status pause();
    r_status startfrompause();
   // enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm);

protected:
    void run();


private:

    std::string musicefile;
    musiccallback callback;
    spacecheck spacecheck_d;
    gettime gettime_d;

    sem_t *Semofnext;
    std::string local_times;

    int ret;
    sigval a;
    int Card;

};

#endif // MP3PLAYER_H
