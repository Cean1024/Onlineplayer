#include "playmp3list.h"
#include <algorithm>


Playmp3list::Playmp3list()
{
    //cachebufm = NULL;
    soundvalu = 80;
    sem_init(&semofnext,0,0);
    number =0;

    audiochard = -1;

}

Playmp3list::~Playmp3list()
{

}

static char playlistfile[namelenght];
static char musicefile[namelenght];


int Playmp3list::checkplaylist(std::string &workfolder)
{

#if 0
    sprintf(tmp,"ls %s/*.mp3 > %s/playlist \n",musicefolder,musicefolder);
    system(tmp);
    sprintf(playlistfile,"%s/playlist",musicefolder);


#ifdef PLAYSTILL
    sem_init(&next_music, 0 , 0);
#endif

    sprintf(tmp,"rm %s >/dev/null 2>&1  ",TMPMUSICFILE);
    system(tmp);
#endif
    return SUCCESS;
}


int Playmp3list::playwithlist(std::vector<std::string> &playlist ,
                              callbackofgetmusicnumber callback ,
                              timeformatdate_t &timedata)
{
    audiochard = timedata.typeofcard;

    DEBUG2INFO("in Playmp3list::playwithlist , card : %d \n",audiochard);

    if ( this->getState() == threadstatusruning ) {
        DEBUG1INFO("another playlist is runing ,stop it\n");
        this->stopplay( );
        usleep ( 10000 );
    }

    workfolder = globaflags::getInstance()->workfolder ;
    workfolder += MUSICESTORAGEFOLDER ;
    workfolder += "/";

    localplaylist = playlist;

    maxplaylistsize = localplaylist.size();
    if( timedata.looptype  != ORDERPLAY ) {

        std::random_shuffle(localplaylist.begin(),localplaylist.end());

    }

    this->loopcount = timedata.loopcount;

    this->sleeptimeafterplayover = timedata.sleepafterplayover;

    this->soundvalu = timedata.volume;

    setcurrentmusic = callback;


    this->start();

    return SUCCESS;
}

int Playmp3list::stopplay( )
{
    DEBUG1INFO("in Playmp3list::stopplay\n");
    number = 0;

    player.mp3stop(audiochard);
    this->distroy();
    localplaylist.clear();
    return SUCCESS;
}

r_status Playmp3list::pause()
{

    if(audiochard < 0) return FAILED;

    player.pause();


    return SUCCESS;
}
r_status Playmp3list::startfrompause()
{

    if(audiochard < 0) return FAILED;

    player.startfrompause();

    return SUCCESS;
}


int Playmp3list::handlsigusr1()
{
    int ret;
    usleep(2000);

    player.mp3stop( audiochard );
/*
    ret = player.checkspace();

    if( ret == FAILED )
        return PLAYSTOP;
*/

    if ( number >= maxplaylistsize ) {
        number = 0;
        loopcount--;
        if(loopcount <=0 )
            return PLAYSTOP;
        sleep(sleeptimeafterplayover);
    }

    printf("number size :%d , max :%d\n",number,maxplaylistsize);
    music = localplaylist[number++];

    setcurrentmusic(music , audiochard);

    music = workfolder + music ;

    player.mp3play ( music , audiochard , semofnext );

    return PLAYSTART;

}

void Playmp3list::handler (int signo)
{
    //printf("handler\n");

    switch(signo) {
    case SIGUSR1:handlsigusr1();break;
    case SwitchOverToNextMusic:handlsigusr1();break;
    default:
        break;
    }
}

int getcmd( )
{
    int ch;
    while(1) {
        ch = getchar();
        if(ch == 27)
            ch = getchar();
        if (ch = 91) {
            ch = getchar();
            return ch;
        }
    }
}

int  Playmp3list::handlercmd()
{
    int ch;
    ch = getcmd();
    switch(ch)
    {
    case 65:

        soundvalu += 10;
        if(soundvalu > 100)soundvalu = 100;
        sound.sound_write(soundvalu);
        std::cout << "up : " <<soundvalu <<" \n"; // top
        return ch;
    case 67:
        std::cout << "left\n"; // left
        //player.mp3stop();
        handler(SwitchOverToNextMusic);
        //player.mp3play ( musicefile , getmusic );
        return ch;
    case 68:
        std::cout << "right\n"; // right
        return ch;
    case 66:

        soundvalu -= 10;
        if(soundvalu < 0) soundvalu = 0;
        sound.sound_write(soundvalu);
         std::cout << "down : " <<soundvalu <<" \n";  // down
        return ch;
    default:
        //std::cout << ch ;
        return ch;
    }
    //usleep(200000);
}

void Playmp3list::run()
{

    sound.sound_write(soundvalu);
    soundvalu = sound.sound_read();
    std::cout << "default sound : " << soundvalu <<" \n";

    handlsigusr1();

    while ( 1 )
    {
        sem_wait( &semofnext );

        if( handlsigusr1() == PLAYSTOP ) {
            printf("Playmp3list stopped!\n");
            break;
        }
    }
}

