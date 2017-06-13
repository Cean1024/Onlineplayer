#include "connnetwork.h"

connnetwork::connnetwork()
{
    audiobufm=NULL;
    audiobufp=NULL;

    pthread_mutex_init(&bufmatrux,NULL);
    sem_init (&this->prepair, 0 , 1);
    sem_init (&this->ready, 0 , 0);
    //audioalsa::getInstance().output
}

connnetwork::~connnetwork()
{

}


int connnetwork::setaudioparam(struct audioparam *params)
{
    params->BITS = 16;
    params->CHANNELS = 2;
    params->RATE = 44100;
    params->DIR = 0;
}

int connnetwork::init(cachebuf *audiobufm, audioparam *params, char *buf)
{
    int ret;

    this->audiocachebuf = audiobufm;
    this->musicfile = buf;

    setaudioparam(params);

    this->audiobufm = audiocachebuf->mbuff;
    this->audiobufp = audiocachebuf->pbuff;
    this->audiobufsize = audiocachebuf->getmaxcachsize();
    this->audioobufsize = audiocachebuf->getobuffsize();
    this->start();

    return ISUCCESS;
}

void connnetwork::fillbuffer()
{
    while( buf + audioobufsize <= bufend ) {
        //printf("filling\n");
        sem_wait(&ready);
        tmpsize = audiocachebuf->getobuffrecivesize();
        //pthread_mutex_lock(&connnetwork::getInstance().bufmatrux);
        memcpy(buf,audiocachebuf->obuff,tmpsize);
        memset(audiocachebuf->obuff,'0',audioobufsize);
        sem_post(&prepair);
        //pthread_mutex_unlock(&connnetwork::getInstance().bufmatrux);
        buf += tmpsize;
    }
}

void connnetwork::run()
{
    while(1) {
        //printf("connetwork: wait inputm\n");
        sem_wait(&audioalsa::getInstance().inputm);
        //printf("fill cache m\n");
        buf =audiobufm;
        bufend = audiobufm + audiobufsize;
        fillbuffer();
        audiocachebuf->setmbuffrecivesize( buf - audiobufm);
        sem_post(&audioalsa::getInstance().outputm);

#ifdef DOUBLECHANNEL
        //printf("connetwork: wait inputp\n");
        sem_wait(&audioalsa::getInstance().inputp);
        //printf("fill cache p\n");
        buf = audiobufp;
        bufend = audiobufp + audiobufsize;
        fillbuffer();
        audiocachebuf->setpbuffrecivesize( buf - audiobufp);
        sem_post( &audioalsa::getInstance().outputp );
#endif
    }
}
