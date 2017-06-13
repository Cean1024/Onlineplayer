#ifndef PLAYMP3LIST_H
#define PLAYMP3LIST_H
#include  <fstream>
#include <fstream>
#include <iostream>
//#include <semaphore.h>
#include <cstdio>

//#include <curses.h>
//#include <ncurses.h>
#include <signal.h>
#include "globaflags.h"
#include "decode/buffercache/cachebuf.h"
#include "decode/soundalsa.h"
#include "mp3player.h"
#include "tools/tools.h"
#include "tools/times/gettime.h"
#include "network/proccessdata.h"
#include "tools/handlesqlite.h"


#define namelenght 200



class Playmp3list:public Thread
{
public:
    Playmp3list();
    ~Playmp3list();

    int playwithlist(std::vector<std::string> &playlist ,
                     callbackofgetmusicnumber callback,
                     timeformatdate_t &timedata);
    int stopplay();

    r_status pause();
    r_status startfrompause();


protected:
    void run();
    int playmusice(char * musicefile);
    int handlercmd();
    int checkplaylist(std::string &workfolder);
    void handler (int signo);
    int handlsigusr1();

private:
    soundalsa sound;
    mp3player player;
    std::vector<std::string> localplaylist;
    int audiochard;
    std::string music;
    std::string workfolder;

    callbackofgetmusicnumber setcurrentmusic;
    sem_t semofnext;
    int number;

    int soundvalu;
    int maxplaylistsize;
    int loopcount;
    int sleeptimeafterplayover;


    //int a;
};

#endif // PLAYMP3LIST_H
