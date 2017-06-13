#include "thransferstation.h"

thransferstation::thransferstation()
{
    audiobufm=NULL;
    audiobufp=NULL;

    pthread_mutex_init(&bufmatrux,NULL);

}

thransferstation::~thransferstation()
{

}

int thransferstation::init ( cachebuf *audiobufm , bufersems_t *bufsems )
{
    int ret;

    this->audiocachebuf = audiobufm;
    this->audiobufm = audiocachebuf->mbuff;
    this->audiobufp = audiocachebuf->pbuff;
    this->audiobufsize = audiocachebuf->getmaxcachsize();
    this->audioobufsize = audiocachebuf->getobuffsize();
    this->inputm = &bufsems->inputm;
    this->outputm = &bufsems->outputm;
    this->inputp = &bufsems->inputp;
    this->outputp = &bufsems->outputp;
    this->prepair = &bufsems->prepair;
    this->ready = &bufsems->ready;
    this->start();
    return ISUCCESS;
}

void thransferstation::fillbuffer()
{
    while( buf + audioobufsize <= bufend ) {
        //printf("filling\n");
        //printf("wait ready\n");
        sem_wait(ready);
        //printf("filling\n");
        tmpsize = audiocachebuf->getobuffrecivesize();
        //pthread_mutex_lock(&connnetwork::getInstance().bufmatrux);
        memcpy(buf,audiocachebuf->obuff,tmpsize);
        //memset(audiocachebuf->obuff,'0',audioobufsize);
        sem_post(prepair);
        //pthread_mutex_unlock(&connnetwork::getInstance().bufmatrux);
        buf += tmpsize;
    }
}

void thransferstation::run()
{
    //printf("thransfer station start\n");
    while(1) {
        //printf("wait inputm\n");
        sem_wait(inputm);
        //printf("fill cache m\n");
        buf =audiobufm;
        bufend = audiobufm + audiobufsize;
        fillbuffer();
        audiocachebuf->setmbuffrecivesize( buf - audiobufm);
        sem_post(outputm);

#ifdef DOUBLECHANNEL
        //printf("wait inputp\n");
        sem_wait(inputp);
        //printf("fill cache p\n");
        buf = audiobufp;
        bufend = audiobufp + audiobufsize;
        fillbuffer();
        audiocachebuf->setpbuffrecivesize( buf - audiobufp);
        sem_post( outputp );
#endif
    }
}
