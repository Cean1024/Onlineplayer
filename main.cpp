#include "coremain.h"


int main(int argc ,char *argv[])
{
    coremain *coremain_t = new coremain();


    coremain_t->display();
    if(argc < 2) {

        printf("useing default  music folder:%s\n",PLAYLISTFOLDER);
        coremain_t->workfolder = PLAYLISTFOLDER;
    } else
        coremain_t->workfolder = argv[1];


    coremain_t->inits();
    coremain_t->exit();

    delete coremain_t;

    return 0;
}

