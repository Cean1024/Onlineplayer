#include "playlistcontrol.h"

playlistcontrol::playlistcontrol()
{

}

playlistcontrol::~playlistcontrol()
{

}

typedef struct playstatus {
    int status;
    int stopflag;
    sem_t sem;
}playstatus_t;

static playstatus_t controlstatus [ SOUNDCARDNUM ];

int playlistcontrol::updatemusic()
{
    Json::Value record;
    //Json::FastWriter wirter;
    std::string sectionid;
    std::vector<std::string > playlistitems;
    char buf[10];

    int size = playlist_control.size();
    for(int i = 0 ; i < size ; i ++ )
    {
        sprintf(buf,"%d",i);
        record = playlist_control[ buf ];
        sectionid = record["id"].asString();
        //sectionid = sectionid.substr( 1,sectionid.size() -3);
        std::cout << "sectionid :" <<sectionid << std::endl;
        manageplaylist( sectionid , TYPEOFPLAYLIST , playlistitems );
        proccessdata::getInstance()->downloadfiles( playlistitems );
        playlistitems.clear();
        usleep(1000);

    }

    size = addplaylist_control.size();

    for(int i = 0 ; i < size ; i ++)
    {
        sprintf(buf,"%d",i);
        record = addplaylist_control[ buf ];
        sectionid = record["id"].asString();
        //sectionid = sectionid.substr(1,sectionid.size() -3);
        std::cout << "sectionid :" <<sectionid << std::endl;
        manageplaylist(sectionid , TYPEOFADDPLAY , playlistitems );
        //for(int j = playlistitems.size() - 1 ; j >= 0 ; j--)
        //    std::cout << "add playlist  : " << playlistitems[j] << std::endl;
        proccessdata::getInstance()->downloadfiles( playlistitems );
        playlistitems.clear();
    }

    DEBUG1INFO ("\nFinish update! \n\n");
}



int playlistcontrol::init()
{

    int ret;

    this->workfolder = globaflags::getInstance()->workfolder;

    sleep(1);

    ret =createallcontrollist();
    if( ret == FAILED ) {
        printf("!!!createallcontrollist failed\n ");
        return IFAILED;
    }

    for(int i =0 ; i < SOUNDCARDNUM ; i++) {
        controlstatus[i].stopflag = 0;
        sem_init( &controlstatus[i].sem ,0 , 0 );
    }

    //

    //return SUCCESS;

    testflag = 0;
    this->start();
    threadplaymp3 = this->getThreadID();
    usleep(1000);

    testflag = 1;
    this->start();
    threadaddplay = this->getThreadID();
    usleep(1000);

    testflag = 2;
    this->start();


    gettime Time;
    //std::string cmd = Time.gettimetickasstring() + " OnlinePlayer启动 播放音乐";
    std::string cmd = "[" + Time.getdateasstring()+" " +Time.gettimeasstring() + "] OnlinePlayer启动 播放音乐";
    LOG_STORAGE(cmd , globaflags::getInstance()->reallogfile );


    return SUCCESS;
}


void playlistcontrol::stopall()
{

    for(int i =0 ; i < SOUNDCARDNUM ; i++) {
        if( controlstatus[i].status == PLAYWAITING ) {
            if( i == MUSICECARD ) {
                playmp3.stopplay();
            } else {
                addplay.stopplay();
            }
        }
        else {
            controlstatus[i].stopflag = 1;
            sem_wait(&controlstatus[i].sem);
            if( i == MUSICECARD ) {
                playmp3.stopplay();
            } else {
                addplay.stopplay();
            }
        }
    }
}

void playlistcontrol::reinit()
{
    stopall();
    pthread_cancel(threadplaymp3);
    pthread_cancel(threadaddplay);
    usleep(20000);
    init();
}


int playlistcontrol::createallcontrollist()
{
    int ret;
    int count = 10;
    while(count -- >0) {
        //sleep(1);
        ret = createcontrollist(TYPEOFPLAYLIST);
        if (ret == FAILED)
            continue;
        else
            break;
    }
    count = 10;
    while( count -- > 0 ) {
        //sleep(1);
        ret = createcontrollist(TYPEOFADDPLAY);
        if ( ret == FAILED )
            continue;
        else
            break;
    }
    std::cout << "update music" <<std::endl;
    updatemusic();

    return ret;

}

int playlistcontrol::getcontrolsection(Json::Value &data , int type)
{
    int ret;
    std::string playlistfile;

    ret = proccessdata::getInstance()->getplaylistfile(playlistfile , type);
    if ( ret == SUCCESS ) {
        ret = sqlite.opendatabase( playlistfile );
        if ( ret == SUCCESS ) {
            ret = sqlite.gettabledata( PLAYLISTCONTORLTABLE , data );
            sqlite.closedatabase();

        } else
            printf("databass [%s] open failed--!\n",playlistfile.c_str());
    } else
        printf("getplaylistfile failed--!\n");

    proccessdata::getInstance()->postplaylistfile(type);

    return ret;

}


void handletimeformate(std::string &tmp ,std::vector<std::string> &splitdates)
{
    int index;
    std::string timestart;
    /*
    index = tmp.find("/",0);
    if( index >= 0 ) {
        timestart = tmp.substr( index + 1, 2 );
        splitdates.push_back(timestart);
    }*/
    index = tmp.find("-",0);
    if(index < 0) {

        timestart = tmp.substr( 0, 8 );
        splitdates.push_back(timestart);

    } else {
        timestart = tmp.substr( 0, index );
        splitdates.push_back(timestart);
        timestart = tmp.substr( index + 1, 8 );
        splitdates.push_back(timestart);
    }
}


int splitejsonwithtimedate(std::vector<std::string> &datestrs,std::vector<std::string> &splitdates)
{
    int size = datestrs.size();
    int index;
    std::string tmp;
    std::string timestart;


    for ( int i = 0 ; i < size ; i++ ) {

        tmp = datestrs[i];

        //DEBUG2INFO ( "datestr : %s\n",tmp.c_str() );

        index = 0;
        index = tmp.find(",",0);

        if( index < 0 ) {

            handletimeformate ( tmp , splitdates );

        } else {
            timestart = tmp.substr( 0,index );
            splitdates.push_back( timestart );
            int tmpindex = index;
            while((index = tmp.find(",",tmpindex + 1)) > 0) {
                timestart = tmp.substr(tmpindex,index);
                handletimeformate(timestart,splitdates);
                tmpindex = index;
            }
        }
    }
    return SUCCESS;

}

int checktimewithoutpoint(std::string &time,std::string &datestr,std::string &intervaltime)
{
    std::string timestart;
    std::string timeend;
    int index;
    index = datestr.find("-",0);
    intervaltime = "0";
    if(index < 0) {
        if( time == datestr )
            return YES;
        else
            return NO;
    } else {

        timestart = datestr.substr(0,index);
        timeend = datestr.substr(index + 1,8);
        printf("timestart time timeend :%s %s %s \n",timestart.c_str(),time.c_str(),timeend.c_str());
        if(timestart <= time && time <= timeend ) {
            index = datestr.find("/",0);

            if( index > 0 ) intervaltime = datestr.substr( index + 1, 2 );

            return YES;
        }
        return NO;

    }

    return NO;
}

int  checktimeindatestr(std::string time,std::string datestr , std::string &intervaltime)
{
    int index;
    int ret;
    std::string tmp;

    index = 0;
    int n_pos = 0;
    while( (index = datestr.find(",",n_pos)) != datestr.npos  ) {

        tmp = datestr.substr( n_pos , index - n_pos );
        ret = checktimewithoutpoint(time,tmp ,intervaltime);
        if(ret == YES ) return ret;
        n_pos = index + 1;

    }
    if( n_pos == 0 ) {
        return checktimewithoutpoint( time , datestr,intervaltime );
    } else {
        tmp = datestr.substr( n_pos , datestr.size() - n_pos );
        ret = checktimewithoutpoint(time , tmp , intervaltime);
    }

    return ret;

}

int playlistcontrol::thransferjsontovecterwithdatestr(Json::Value &datas, std::vector<std::string> &datestrs)
{
    int size;
    char buf[16];
    std::string tmp;
    Json::Value onerecord;

    size = datas.size();

    datestrs.clear();

    for ( int i = 0 ; i < size ; i++ ) {

        sprintf(buf,"%d",i);
        onerecord = datas[buf];
        if( checkdatevalidity( onerecord ) == NO ) continue;
        tmp = onerecord ["datestr"].asString();
        datestrs.push_back( tmp );
        // DEBUG2INFO( "datestr : %s\n",tmp.c_str() );

    }
}

int playlistcontrol::handlebigainandendtick(Json::Value &record)
{
    std::string currentdate;
    std::string beginedate = record["begintick"].asString();
    std::string enddate = record["endtick"].asString();

    currentdate = Time.getdateasstring();
    //std::cout << beginedate <<":" <<currentdate<<":" << enddate<< std::endl;
    if( beginedate <= currentdate && currentdate <= enddate)

        return YES;

    return NO;
}

int playlistcontrol::handleweekdaydate (std::string &weekdaydate)
{

    int index;

    std::string tmp;
    std::string currentweekday;

    Time.getweekday(currentweekday);

    index = 0;
    int n_pos = 0;
    while( (index = weekdaydate.find(",",n_pos) ) != weekdaydate.npos  ) {

        tmp = weekdaydate.substr( n_pos , index - n_pos );

        if(tmp == currentweekday ) return YES;
        n_pos = index + 1;
    }
    if( n_pos == 0 ) {
        if(tmp == weekdaydate ) return YES;
        return NO;
    } else {
        tmp = weekdaydate.substr( n_pos , weekdaydate.size() - n_pos );
        if(tmp == currentweekday ) return YES;
    }

    return NO;

}

int comparedayandmday ( std::string &day,std::string &mouthdate )
{
    int index;
    std::string start;
    std::string stop;

    int size = mouthdate.size();

    if(size > 2) {

        index = mouthdate.find("-",0);
        start = mouthdate.substr(0,index);
        stop  = mouthdate.substr( index + 1 , size - index );

        if(start <= day && day <= stop)
            return YES;
        return NO;

    }

    if( day == mouthdate )
        return YES;

    return NO;
}

int playlistcontrol::handlemouthtypedate(std::string &mouthtypedate)
{
    int index;

    std::string tmp;
    std::string currentdayofmouth;

    Time.getdayofmouth(currentdayofmouth);
    index = 0;
    int n_pos = 0;
    while( (index = mouthtypedate.find(",",n_pos) ) != mouthtypedate.npos  ) {

        tmp = mouthtypedate.substr( n_pos , index - n_pos );

        if( comparedayandmday(currentdayofmouth,tmp) == YES) return YES;

        n_pos = index + 1;
    }
    if( n_pos == 0 ) {

        if( comparedayandmday(currentdayofmouth,mouthtypedate) == YES) return YES;

        return NO;
    } else {
        tmp = mouthtypedate.substr( n_pos , mouthtypedate.size() - n_pos );
        if( comparedayandmday(currentdayofmouth,tmp) == YES) return YES;
    }

    return NO;

}

int dealwithmouthdateofyear(std::string &currentmouthofyear , std::string &mouthdateofyear)
{
    int size = mouthdateofyear.size();
    if(size > 5) {
        std::string daystart = mouthdateofyear.substr(0,5);
        std::string daystop = mouthdateofyear.substr(6,5);

        if(daystart <= currentmouthofyear && currentmouthofyear <= daystop) return YES;
        return NO;
    }
    if(size == 4){
        std::string mouth = currentmouthofyear.substr(0,2);
        std::string mouthofset = mouthdateofyear.substr(0,2);
        if( mouth == mouthofset) return YES;
        return NO;
    }
    if(currentmouthofyear == mouthdateofyear)return YES;

    return NO;
}


int playlistcontrol::handleyeartypedate(std::string &yeartypedate)
{
    int index;

    std::string tmp;
    std::string currentmouthofyear;

    currentmouthofyear = Time.getdateasstring( );
    index = currentmouthofyear.find( "-" , 0 );
    currentmouthofyear =  currentmouthofyear.substr(index + 1, currentmouthofyear.size() - index);

    index = 0;
    int n_pos = 0;


    while ( (index = yeartypedate.find(",",n_pos) ) != yeartypedate.npos  ) {
        tmp = yeartypedate.substr( n_pos , index - n_pos );
        if (dealwithmouthdateofyear(currentmouthofyear,tmp) == YES ) return YES;
        n_pos = index + 1;
    }

    if( n_pos == 0 ) {

        if (dealwithmouthdateofyear(currentmouthofyear,yeartypedate) == YES ) return YES;

        return NO;
    } else {
        tmp = yeartypedate.substr( n_pos , yeartypedate.size() - n_pos );
        if (dealwithmouthdateofyear(currentmouthofyear,yeartypedate) == YES ) return YES;
    }

    return NO;

}

int playlistcontrol::checkdatevalidity(Json::Value &record)
{
    std::string datetype;
    int ret;

    ret = handlebigainandendtick(record);
    if( ret != YES )
        return NO;

    datetype = record["datetype"].asString();
    sscanf(datetype.c_str(),"%d",&ret);
    switch(ret) {
    case 1:return YES;
    case 2:

        datetype = record["dateparam"].asString();
        return handleweekdaydate(datetype);

    case 3:
        datetype = record["dateparam"].asString();
        return handlemouthtypedate(datetype);

    case 4:
        datetype = record["dateparam"].asString();
        return handleyeartypedate(datetype);

    default:return NO;
    }
}

int playlistcontrol::getplaylistcontroltime(Json::Value &datas, std::vector<std::string> &datestrs ,
                                            std::vector<std::string> &splitedatestr,
                                            timeformatdate_t &timedata)
{
    std::string tmp;
    std::string levela;
    std::string levelb;
    Json::Value onerecord;
    int ret;
    int size;
    char buf[16];
    int foundflag = 0;

    timedata.localtime = Time.gettimeasstring();

    DEBUG2INFO( "localtime : %s\n",timedata.localtime.c_str() );

    size  = splitedatestr.size();

    if ( timedata.localtime < splitedatestr[0] ) {

        timedata.waittime  = splitedatestr[0];

        return WAITING;

    }

    if( timedata.localtime> splitedatestr[size - 1]) {

        timedata.waittime  = "24:00:00";

        return WAITING;
    }

    for(int i =0 ; i< size - 1; i++) {

        DEBUG2INFO( "datestr : %s\n",splitedatestr[i].c_str() );

        if( splitedatestr[i] <= timedata.localtime && timedata.localtime < splitedatestr[i +1] ) {

            timedata.timestart = splitedatestr[i];
            timedata.timestop  = splitedatestr[i +1];
            timedata.waittime  = splitedatestr[i +1];

            break;
        }
    }

    size = datestrs.size();
    timedata.record.clear();

    for( int i = 0 ; i < size ; i++ ) {

        tmp = datestrs[ i ];
        ret = checktimeindatestr( timedata.localtime , tmp , timedata.intervaltime );
        if(ret == YES) {

            sprintf(buf,"%d",i);
            onerecord =  datas[ buf ];

            levela = onerecord["level"].asString();
            levelb = timedata.record["level"].asString();
            if( levela > levelb ) {
                foundflag++;
                timedata.record = onerecord;
                tmp = onerecord["looptype"].asString();
                sscanf(tmp.c_str(),"%d",&timedata.looptype);
                tmp = onerecord["loopcount"].asString();
                sscanf(tmp.c_str(),"%d",&timedata.loopcount);
                tmp = onerecord["volume"].asString();
                sscanf(tmp.c_str(),"%d",&timedata.volume);
                tmp = onerecord["id"].asString();
                sscanf(tmp.c_str(),"%d",&timedata.recordid);
                int tmpint;
                sscanf(timedata.intervaltime.c_str(),"%d",&tmpint);
                timedata.sleepafterplayover = tmpint * 60;
            }
        }
    }

    if( foundflag > 0 )
        return SUCCESS;

    return WAITING;


}


int playlistcontrol::SortJsonDateWithTime(Json::Value &unsortdataa , Json::Value &unsortdatab,Json::Value &resoult)
{

    Json::Value dataa;
    Json::Value datab;
    //Json::FastWriter writer;
    std::string timea;
    std::string timeb;

    int finishflag = 0; // 0 both unfinish , 1  one finish;

    int sizea = unsortdataa.size();
    int sizeb = unsortdatab.size();
    int indexa = 1;
    int indexb = 1;
    char buf[10];


    int indexresoult = 1;
    while(1) {

        switch ( finishflag ) {
        case 0:{
            sprintf(buf,"%d",indexa);
            dataa= unsortdataa[buf];

            sprintf(buf,"%d",indexb);
            datab= unsortdatab[buf];
            timea = writer.write(dataa["datestr"]);
            timeb = writer.write(datab["datestr"]);
            if(timea < timeb) {
                if( indexa <= sizea ) {
                    sprintf(buf,"%d",indexresoult++);
                    resoult[buf] = dataa;
                    indexa++;
                } else {
                    finishflag = 1;
                }
            } else {
                if( indexb <= sizeb ) {
                    sprintf(buf,"%d",indexresoult++);
                    resoult[buf] = datab;
                    indexb++;
                } else {
                    finishflag = 2;
                }
            }
        };break;
        case 1:{
            sprintf(buf,"%d",indexb);
            datab= unsortdatab[buf];
            if( indexb <= sizeb ) {
                sprintf(buf,"%d",indexresoult++);
                resoult[buf] = datab;
                indexb++;
            } else {
                finishflag = 3;
            }
        };break;

        case 2:{
            sprintf(buf,"%d",indexa);
            dataa= unsortdataa[buf];
            if( indexa <= sizea ) {
                sprintf(buf,"%d",indexresoult++);
                resoult[buf] = dataa;
                indexa++;
            } else {
                finishflag = 3;
            }
        };break;

        case 3:return SUCCESS;
        default:return FAILED;
        }

    }
    return FAILED;

}


int playlistcontrol::createcontrollist ( int type )
{

    int ret;


    if(type == TYPEOFPLAYLIST ) {
        ret = getcontrolsection ( playlist_control , TYPEOFPLAYLIST );
        if(ret == FAILED){
            printf(" get TYPEOFPLAYLIST controlsection failed!\n");
            return ret;
        }
    } else {
        ret = getcontrolsection( addplaylist_control , TYPEOFADDPLAY );
        if ( ret == FAILED ){
            printf(" get TYPEOFADDPLAY controlsection failed!\n");
            return ret;
        }
    }

    return SUCCESS;
}


int playlistcontrol::getplaylistitems(Json::Value &data, std::string sectionid, int type)
{
    int ret;
    std::string playlistfile;

    ret = proccessdata::getInstance()->getplaylistfile(playlistfile , type);
    if ( ret == FAILED )
        return ret;

    ret = sqlite.opendatabase( playlistfile );
    if ( ret == FAILED ) {
        proccessdata::getInstance()->postplaylistfile(type);
        return ret;
    }

    ret = sqlite.getdatafromedb("playlist_item","section" , sectionid,data);

    sqlite.closedatabase();
    proccessdata::getInstance()->postplaylistfile(type);

    return ret;

}

int  playlistcontrol::manageplaylist( std::string &sectionid , int type ,
                                      std::vector<std::string> &sectionplaylist )
{
    Json::Value dbdata;
    Json::Value onedata;
    Json::StyledWriter writer;
    std::string tmp;

    int size;
    char buf[10];

    size = getplaylistitems( dbdata , sectionid ,type );
    if (size == FAILED)
        return size;

    size = dbdata.size();
    sectionplaylist.clear();
    for( int i =0 ; i < size ; i++ ) {
        sprintf(buf,"%d",i);
        onedata = dbdata[ buf ];
        tmp = onedata["fileid"].asString();

        //tmp = workfolder + MUSICESTORAGEFOLDER + "/" + tmp;
        sectionplaylist.push_back(tmp);
        //std::cout <<  tmp << std::endl;
    }

    return SUCCESS;


}

uint playlistcontrol::getsleeptime( std::string stoptime)
{
    std::string localtime;

    localtime = Time.gettimeasstring();
    uint sleeptime = Time.calculatesubtime(stoptime,localtime);
    //DEBUG2INFO( "sleep time : %d\n" , sleeptime );

    return sleeptime;

}
#define WAITESTOP 0
#define WAITESTART 1
static int numberofplay = 0;

void playlistcontrol::mssleep(std::string time , int type)
{
    uint waittime;
    struct timeval temp;

    if(type == WAITESTART) {
        numberofplay++;
        if(numberofplay == 2) {

            globaflags::getInstance()->inputcurrentfileid("0");

        }
    }

    waittime = getsleeptime ( time );

    temp.tv_usec = 0;
    temp.tv_sec = waittime;

    select(0, NULL, NULL, NULL, &temp);

    if( type = WAITESTART )
        numberofplay--;

}


void setmusicidofplaylist( std::string &music , int type )
{
    HandleSqlite sqlite;
    int ret;
    std::string playlistfile;
    Json::Value data;
    gettime Time;

    if ( controlstatus[type].stopflag ) sem_post( &controlstatus[type].sem );


    ret = proccessdata::getInstance()->getplaylistfile(playlistfile , type);
    if ( ret == FAILED )
        return ;

    ret = sqlite.opendatabase( playlistfile );
    if ( ret == FAILED ) {
        proccessdata::getInstance()->postplaylistfile(type);
        return ;
    }

    ret = sqlite.getdatafromedb ("playlist_item", "fileid" , music , data);
    sqlite.closedatabase();
    proccessdata::getInstance()->postplaylistfile(type);

    data = data["0"];

    playlistfile = data["id"].asString();
    globaflags::getInstance()->inputcurrentfileid(playlistfile);

    playlistfile = data["title"].asString();

    ret = playlistfile.find("(",0);
    if(ret != -1) playlistfile = playlistfile.replace(ret,1,"[");

    ret = playlistfile.find(")",0);
    if(ret != -1) playlistfile = playlistfile.replace(ret,1,"]");

    ret = playlistfile.find("'",0);
    if(ret != -1) playlistfile = playlistfile.replace(ret,1,"~");


    //playlistfile = "\" " +Time.gettimetickasstring() + " 播放歌曲 " + playlistfile +"\"";
    playlistfile = "\"[" + Time.getdateasstring()+" " +Time.gettimeasstring() + "] 播放歌曲 " + playlistfile +"\"";
    //std::cout <<"echo "<< playlistfile <<" >> " << globaflags::getInstance()->reallogfile << std::endl;
    LOG_STORAGE( playlistfile , globaflags::getInstance()->reallogfile );

}

int playlistcontrol::createplaylist(timeformatdate_t &timedata, int type )
{
    std::vector<std::string> sectionplaylist;
    //char buf[ 10 ];
    std::string sectionid;

    Playmp3list * playmp3list;

    if( timedata.prerecordid != timedata.recordid ) {
        sectionid = timedata.record["id"].asString();

        if(type == TYPEOFPLAYLIST) {
            playmp3list = &playmp3;
        } else {
            playmp3list = &addplay;

            if( playmp3.getState() == threadstatusruning )
                playmp3.pause();
        }

        DEBUG1INFO ( "geting playlist\n" );


        manageplaylist ( sectionid , type , sectionplaylist );

        timedata.typeofcard = type;

        /*start play*/
        playmp3list->playwithlist ( sectionplaylist  , setmusicidofplaylist , timedata );

        sectionplaylist.clear();

        timedata.prerecordid = timedata.recordid;
        }

        DEBUG2INFO( "sleep till : %s\n" , timedata.timestop.c_str() );
        mssleep( timedata.timestop , WAITESTOP );

        printf("wake up!\n");

    /*stop play*/
    //playmp3list.stopplay();

    return SUCCESS;

}

void playlistcontrol::handleplaylist()
{
    int ret;

    timeformatdate_t controldata;
    std::vector<std::string> datestrs;
    std::vector<std::string> splitedatestr;
    DEBUG1INFO( "in handleplaylist\n" );

    controldata.prerecordid = -1;

    thransferjsontovecterwithdatestr( playlist_control , datestrs );

    splitejsonwithtimedate( datestrs , splitedatestr);

    while(1) {
        ret = getplaylistcontroltime ( playlist_control , datestrs , splitedatestr , controldata  );
        if ( ret == FAILED ) {
         DEBUG1INFO ("getplaylistcontroltime failed!\n");
         break;
        }

        switch( ret ) {

        case SUCCESS: {
            controlstatus[MUSICECARD].status = PLAYRUNING;
            createplaylist(controldata, TYPEOFPLAYLIST );

        };break;
        case WAITING: {
            if ( playmp3.getState() == threadstatusruning ) playmp3.stopplay();
            controlstatus[MUSICECARD].status = PLAYWAITING;
            DEBUG2INFO ( "playlist waiting  till : %s\n" , controldata.waittime.c_str() );
            mssleep ( controldata.waittime , WAITESTART );

        };break;

        }
    }
}

void playlistcontrol::handleaddplaylist()
{
    int ret;

    timeformatdate_t controldata;
    std::vector<std::string> datestrs;
    std::vector<std::string> splitedatestr;

    controldata.prerecordid = -1;

    //sleep(3);
    DEBUG1INFO("in handleaddplaylist\n");

    thransferjsontovecterwithdatestr( addplaylist_control , datestrs );

    splitejsonwithtimedate( datestrs , splitedatestr );

    while(1) {

        ret = getplaylistcontroltime ( addplaylist_control , datestrs ,splitedatestr, controldata  );

        if ( ret == FAILED )
            break;

        switch( ret ) {

        case SUCCESS: {
            controlstatus[INTERCUTCARD].status = PLAYRUNING;
            createplaylist ( controldata , TYPEOFADDPLAY);

        };break;
        case WAITING: {

            if ( addplay.getState() == threadstatusruning ) addplay.stopplay();
            controlstatus[INTERCUTCARD].status = PLAYWAITING;
            playmp3.startfrompause();
            DEBUG2INFO("addplaylist waiting till : %s\n",controldata.waittime.c_str());
            mssleep( controldata.waittime , WAITESTART );

        };break;

        }
    }
}

void playlistcontrol::upgradeplaylist()
{

    //this->UpgradeTime = time;
    testflag = 2;
    this->start();
    return;

}

void playlistcontrol::controlplaylists()
{

    //mssleep( UpgradeTime , WAITESTOP );

    sem_wait(&globaflags::getInstance()->semofupgradeplaylist);

    if ( createallcontrollist() == FAILED ) {

        printf ("!!!createallcontrollist failed\n ");
        return ;

    }

    reinit();

    cleanrom.cleanrom(workfolder);

}

void playlistcontrol::run()
{
    int ret = testflag;

    switch(ret) {
    case 0: handleplaylist();break;
    case 1: handleaddplaylist();break;
    case 2: controlplaylists();break;
    default:break;
    }
}
