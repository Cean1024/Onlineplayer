#ifndef PROCCESSDATA_H
#define PROCCESSDATA_H
#include <time.h>

#include <iostream>
#include <fstream>
#include "tools/times/gettime.h"
#include "tools/tools.h"
#include "globaflags.h"
#include "httpclient.h"
#include "tools/handlesqlite.h"
#include "tools/tools.h"
#include "tools/md5sum.h"
//#include "playlistcontrol.h"
//#include "rommanage/managerom.h"


#define DEFAULTPLAYLISTFILE "backupplaylistname"
#define DEFAULTADDPLAYLISTFILE "backupaddplayname"


struct middleparam{
    std::string a;
    std::string b;
    std::string filename;
};


class proccessdata:public Thread
{
public:
    proccessdata();
    ~proccessdata();

    static proccessdata * getInstance(){
     static proccessdata Proccessdata;
     return &Proccessdata;
    }

    sem_t semofprocessdata;
    sem_t semofdownload;

    int init();
    void setheartbeatdata(Json:: Value &data);
    Json:: Value getheartbeatdata();
    int getplaylistfile(std::string &playlist,int type );
    void postplaylistfile(int type);
    int downloadfiles(std::vector<std::string> &fileids);


protected:
    void run();
    //void * threadrunbody(void * p);

    int getplaylist(std::string &playlistid ,int type);
    int playlistcheck(std::string &playlistid , std::string &timetick ,int type);
    int checkplaylist(std::string &workfolder );
    void handleudpheartbeatdata();

    int StoragePlaylistUpdateTime(std::string &timea ,std::string &timeb);
    int GetPlaylistUpdateTime(std::string &timea ,std::string &timeb);
    int creatlocaldbfile();
    int checkdownloadfilefold(std::string &fileid);
    int downloadonefile(std::string &url , std::string &filepathandname);
    void handledownloadfiles();

    void uploadlog();
    void getnotify();

private:
    pthread_mutex_t MutexOfJsondata;
    pthread_mutex_t MutexOfPlaylist;
    pthread_mutex_t MutexOfHttp;
    pthread_mutex_t MutexOfDownload;
    pthread_mutex_t MutexOfAddPlaylist;
    Json::Value heartbeatdata;
    //HttpClient Client;

    std::vector<std::string> fileids;

    std::string serverip;
    std::string deviceid;
    std::string workfolder;
    std::string playlistpathandname;
    std::string addplaypathandname;
    std::string updatetickofplaylist;
    std::string updatetickofaddplaylist;

    Tools tools;
    Md5sum Summd5;
    //ManageRom cleanrom;

    int functionselect;

};

#endif // PROCCESSDATA_H
