#ifndef CONNNETWORK_H
#define CONNNETWORK_H


#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>
#include "globaflags.h"
#include "thread.h"
#include "decode/buffercache/cachebuf.h"
#include "decode/audio.h"
#include "network/protocal.h"

//#define buffersize 4096




class connnetwork:public Thread
{
public:
    connnetwork();
    ~connnetwork();
    static connnetwork &getInstance(){
     static connnetwork Connnetwork;
     return Connnetwork;
    }
    pthread_mutex_t bufmatrux;
    sem_t prepair;
    sem_t ready;

    char * musicfile;
    int init(cachebuf *audiobufm, struct audioparam *params, char *buf);

protected:
    void run();
    int setaudioparam(struct audioparam *params);
    void fillbuffer();


private:
    char *audiobufm;
    char *audiobufp;
    cachebuf *audiocachebuf;
    unsigned int audiobufsize;
    unsigned int audioobufsize;
    char * mbuff;

    char *buf;
    char *bufend;
    unsigned int tmpsize;

};

#endif // CONNNETWORK_H
