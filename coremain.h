#ifndef COREMAIN_H
#define COREMAIN_H

#include "decode/audiooss.h"
//#include "alsaapi.h"
#include "network/netclient.h"
#include "network/httpclient.h"
#include "network/checknetwork.h"
#include "network/heartbeat.h"
#include "playlistcontrol.h"
#include "network/proccessdata.h"
#include "update/update.h"


#define SVersion "V1.0.0T"
#define SName "OnlinePlayer"

class coremain
{

public:
    coremain();
    ~coremain();
    void display(void);
    int inits(void);
    void exit(void);
    char * workfolder;

};

#endif // COREMAIN_H
