#ifndef CHECKNETWORK_H
#define CHECKNETWORK_H
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "globaflags.h"
#include "decode/audiothreadcheck.h"
class checknetwork:public Thread
{
public:
    checknetwork();
    ~checknetwork();

    static checknetwork * getInstance(){

        static checknetwork Cnetwork;

     return & Cnetwork;
    }

    int init();
    int getnetworkstatus();
    void setnetworkstatus(statusofnetwork status);

protected:
    void run();

private:
    pthread_mutex_t NetworkStatusFlag;
    int NetworkStatus;
};

#endif // CHECKNETWORK_H
