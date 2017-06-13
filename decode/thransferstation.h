#ifndef THRANSFERSTATION_H
#define THRANSFERSTATION_H
#include <pthread.h>
#include <semaphore.h>
#include "thread.h"
#include "decode/buffercache/cachebuf.h"

typedef struct bufersems{
    sem_t prepair;
    sem_t ready;
    sem_t inputm;
    sem_t outputm;
    sem_t inputp;
    sem_t outputp;
}bufersems_t;

class thransferstation:public Thread
{
public:
    thransferstation();
    ~thransferstation();

    pthread_mutex_t bufmatrux;
    sem_t *prepair;
    sem_t *ready;
    sem_t *inputm;
    sem_t *outputm;
    sem_t *inputp;
    sem_t *outputp;

    char * musicfile;
    int init(cachebuf *audiobufm ,bufersems_t *bufsems);

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

#endif // THRANSFERSTATION_H
