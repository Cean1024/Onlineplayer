#ifndef GLOBAFLAGS_H
#define GLOBAFLAGS_H
#include "thread.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <semaphore.h>
#include "tools/debug/debug.h"
#include "include/json/json.h"

#define PLAYLISTFOLDER  "/home/samba/music"


/**************audio config***************/
#define CONFIG_AUDIOALSA
#define BUFFSIZE 3072
#define DOUBLECHANNEL
#define MAXLEN_AUDIODATACOUNT 1
#define DEFAULTCOUNTBYTE 4
#define SOUNDCARDNUM 2
#define MUSICECARD ( SOUNDCARDNUM - 1 )
#define INTERCUTCARD 0

#define TYPEOFPLAYLIST MUSICECARD
#define TYPEOFADDPLAY 0
/****************************************/


/***************http config****************/
#define HTTPLOGINURL            "/index.php?m=home&c=index&a=s_login&p=json"
#define HTTPGETPLAYLISTURL      "/index.php?m=music&c=playlist&a=s_playlist&type=sqlite&id="
#define HTTPGETFILESERVERURL    "/index.php?m=home&c=fileserver&a=s_action&p=json"
#define HTTPDOWNLOADFILEURL     "/index.php?m=music&c=file&a=s_down&key=key&token=token&format=mp3&fileid="
#define HTTPGETNOTICEURL        "/index.php?m=home&c=notify&a=s_reads"
#define HTTPUPLOADURL           "/index.php?m=home&c=log&a=s_logupload"
/******************************************/


/*************playlist config**************/
#define PLAYLISTCONTORLTABLE    "playlist_section"
#define PLAYLISTITEMTABLE       "playlist_item"
#define PLAYLISTNAMETABLE       "playlist"

#define ORDERPLAY   0
#define RANDOMPLAY  1

typedef void (*callbackofgetmusicnumber)(std::string &music , int type);

/******************************************/


/****************device config*************/
#define LOGFILE "/Log.d"
#define DBFILESTORAGEFOLDER "/playlist"
#define MUSICESTORAGEFOLDER "/download"

#define LOCALDBFILENAME "/var/run/localdbfile.db3"
#define LOCALDUPDATETIMEFILE "/UpdateTimeFile"

#define DEVICEIDFOLDER "/fileofdevice"
/*************************************/

enum keansthreadstatus {
    threadstatusexit=0,
    threadstatusnew,
    threadstatusruning
};
typedef  int r_status;
enum returnstatus {

    ERROR = -2,
    FAILED = -1,
    SUCCESS=0,
    PSUCCESS,
    PFAILED,
    FSUCCES,
    FFAILD,
    ISUCCESS,
    IFAILED,
    IGENOR,
    NEEDUPDATA,
    NOTNEEDUPDATA,
    EXIST,
    NOTEXIST,
    WAITING,
    YES,
    NO,
    PLAYSTART,
    PLAYSTOP,
    PLAYRUNING,
    PLAYWAITING
};

enum calltype {
    callwait = 0,
    callnotwait,
};

enum statusofnetwork {
    networkonline = 0,
    networkoffline,
    serveronline,
    serveroffline
};

enum statusofplaymusic {
    playok = 10,
    playerror,
    playback,
    playnext
};

enum platformselect {
    platformisrespberry = 0,
    platformisi386
};

enum user_signal{
    SwitchOverToNextMusic = 1024,
    SwitchOverToLastMusic
};



typedef struct Server_Status {
   int ServerStatusFlag;
   time_t ServerLoginTime;
   time_t heartbeatTime;
}Server_Status_t;


/*
 * playlist control struct
 *
 */
typedef struct timeformatdate {

    Json::Value record;
    std::string timestart;
    std::string timestop;
    std::string localtime;
    std::string waittime;
    std::string intervaltime;
    int recordid;
    int prerecordid;
    int looptype;
    int loopcount;
    int volume;
    int datetype;
    int dateparam;
    int playtype;
    int typeofcard;
    int sleepafterplayover;

}timeformatdate_t;


class globaflags
{
public:
    globaflags();
    ~globaflags();

    static globaflags * getInstance(){
     static globaflags Globaflags;
     return &Globaflags;
    }

    std::string deviceid;
    std::string serverip;
    int udpport;
    int tcpport;
    std::string workfolder;
    std::string reallogfile;
    std::string LogUploadFlag;

    sem_t semofupgradeplaylist;



    int init(char *workfolder);
    void setserverstatus(Server_Status_t &status);
    Server_Status_t getserverstatus();
    int getplatformflag();

    void inputcurrentfileid(std::string fileid);
    int outputcurrentfileid(std::string &fileid);


private:
    Server_Status_t Serverstatus;

    int BoardPcFlag;
    std::string fileid;
    pthread_mutex_t MutexOfServer;
    pthread_mutex_t MutexOfFileid;

};

#endif // GLOBAFLAGS_H
