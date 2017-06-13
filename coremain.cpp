#include "coremain.h"

coremain::coremain()
{

}

void coremain::display(void)
{
    printf("**************************************\n");
    printf("* SoftwareName:%s\n",SName);
    printf("* Version:%s\n",SVersion);
    printf("* CompileTime: %s %s\n",__DATE__,__TIME__);
    printf("**************************************\n");
}

int coremain::inits(void)
{
    bool ret;
    //printf("start coremain inits\n");

    //checknetwork::getInstance()->init();                // check if network is ok;

    globaflags::getInstance()->init( workfolder );

    proccessdata::getInstance()->init();

    //heartbeat::getInstance()->init();

    Update::getInstance()->init();

    playlistcontrol::getInstance()->init();

    return SUCCESS;
}

void coremain::exit( void )
{
    printf("coremain wait to exit\n");
    while ( 1 )
    {
        sleep(1);
    }
}
coremain::~coremain()
{

}

