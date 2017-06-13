#ifndef PLAYLISTCONTROL_H
#define PLAYLISTCONTROL_H
#include "globaflags.h"
#include "playmp3list.h"
#include <sys/select.h>
#include <algorithm>
#include "rommanage/managerom.h"




class playlistcontrol:public Thread
{
public:
    playlistcontrol();
    ~playlistcontrol();

    static playlistcontrol * getInstance(){
     static playlistcontrol PPlaylistcontrol;
     return &PPlaylistcontrol;
    }

    int init();
    void stopall();
    void reinit();
    void upgradeplaylist();

protected:
    void run();

    int updatemusic();
    int createallcontrollist();
    int createcontrollist(int type);
    int getcontrolsection( Json::Value &data, int type );
    int getplaylistitems(Json::Value &data, std::string sectionid, int type);

    int getplaylistcontroltime(Json::Value &datas, std::vector<std::string> &datestrs, std::vector<std::string> &splitedatestr, timeformatdate_t &timedata );
    int SortJsonDateWithTime(Json::Value &unsortdataa,
                                              Json::Value &unsortdatab,
                                              Json::Value &resoult);
    int thransferjsontovecterwithdatestr(Json::Value &datas,
                                         std::vector<std::string> &datestrs);
    int handlebigainandendtick(Json::Value &record);
    int handleweekdaydate(std::string &weekdaydate);
    int handlemouthtypedate(std::string &mouthtypedate);
    int handleyeartypedate(std::string &yeartypedate);
    int checkdatevalidity(Json::Value &record);

    int  getitemsformdatabase();
    uint getsleeptime(std::string stoptime);
    void mssleep(std::string time , int type);
    int  manageplaylist(std::string &sectionid, int type ,
                        std::vector<std::string> &sectionplaylist);
    int  createplaylist(timeformatdate_t &timedata , int type );
    void handleplaylist();
    void handleaddplaylist();
    void controlplaylists();

private:
    HandleSqlite sqlite;
    gettime Time;
    ManageRom cleanrom;

    std::string workfolder;

    Json::FastWriter writer;
    Json::Value playlist_control;
    Json::Value playlist_item;
    Json::Value addplaylist_control;
    Json::Value addplaylist_item;

    pthread_t threadplaymp3;
    pthread_t threadaddplay;

    int testflag;

    Playmp3list playmp3;
    Playmp3list addplay;

    std::string UpgradeTime;


};

#endif // PLAYLISTCONTROL_H
