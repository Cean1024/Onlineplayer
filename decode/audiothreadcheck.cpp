#include "audiothreadcheck.h"

audiothreadcheck::audiothreadcheck()
{

}

audiothreadcheck::~audiothreadcheck()
{

}

void audiothreadcheck::run()
{
    int status;
    while(1) {
        sleep(3);
        //status = audioalsa::getInstance()->getState();
       // if(status == threadstatusexit )
        //    audioalsa::getInstance()->start();
    }
}

