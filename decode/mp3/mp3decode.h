#ifndef MP3DECODE_H
#define MP3DECODE_H
#include "include/mad/mad.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>

//#include "network/connnetwork.h"
#include "alsaapi.h"
//#include "decode/buffercache/cachebuf.h"
typedef int  ( * printffunc )(char * buf, unsigned int size);
typedef enum mad_flow ( * madoutput )(void *data,struct mad_header const *header,struct mad_pcm *pcm);


class mp3decode
{
public:
    mp3decode();
    ~mp3decode();
    int play(const char *music);
    int stop();
    r_status registe_output(madoutput outputfunc);
    r_status pause(int card);
    r_status startfrompause(int card);


private:

    struct mad_decoder decoder;
    struct stat stat_d;
    int stopflag;
    void *fdm;
    int filefd;
    madoutput outputfunc;

    int decode(unsigned char const *start, unsigned long length);

};

#endif // MP3DECODE_H
