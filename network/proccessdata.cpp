#include "proccessdata.h"

proccessdata::proccessdata()
{
    pthread_mutex_init(&MutexOfJsondata,NULL);
    pthread_mutex_init(&MutexOfPlaylist,NULL);
    pthread_mutex_init(&MutexOfAddPlaylist,NULL);
    pthread_mutex_init(&MutexOfHttp,NULL);
    pthread_mutex_init(&MutexOfDownload,NULL);
    sem_init( &semofprocessdata, 0 , 0);
    sem_init( &semofdownload , 0 , 0 );
}

proccessdata::~proccessdata()
{

}

static HttpClient Client;

#define heartbeatdatalength 7

static std::string recvdataformate [ heartbeatdatalength ] = {
    "status","playlist","playlist_update","addplaylist",
    "addplaylist_update","notify","debug_log"};


int proccessdata::init()
{

    this->serverip = globaflags::getInstance()->serverip;
    this->deviceid = globaflags::getInstance()->deviceid;
    this->workfolder = globaflags::getInstance()->workfolder;
    this->playlistpathandname = "NULL";
    this->addplaypathandname = "NULL";

    creatlocaldbfile();

    this->functionselect = 0;
    this->start();
    usleep(10000);

    //this->functionselect = 1;
    //this->start();
    return SUCCESS;
}

int proccessdata::creatlocaldbfile()
{
    HandleSqlite sqlite;
    std::string cmd;

    std::vector<std::string> files;

    cmd ="rm ";
    cmd = cmd + LOCALDBFILENAME + " >/dev/null 2>&1";
    system(cmd.c_str());
    cmd = LOCALDBFILENAME;
    sqlite.opendatabase(cmd);
    cmd = "create table playlist_item(id interger primary key, fileid text ,stat interger);";
    sqlite.runsqlitecmd(cmd);
    cmd = "cd " +  workfolder + MUSICESTORAGEFOLDER + " && " + "ls" +" && cd -";

    int size = tools.myexec(cmd,files);
    char buf[10];
    for(int i = 0 ; i < size -1 ; i++) {
        sprintf(buf,"%d",i);
        cmd = buf;
        cmd = "Insert into playlist_item(id, fileid, stat) values(" + cmd + ",\"" + files[i] + "\",0);";
        //printf("%s\n",files[i].c_str());
        sqlite.runsqlitecmd( cmd );
        //usleep(1000);
    }
    //cmd = "select * from playlist_item";
    //sqlite.runsqlitecmd(cmd);
    sqlite.closedatabase();
    return SUCCESS;

}



int proccessdata::downloadfiles(std::vector<std::string> &fileids)
{
    pthread_mutex_lock( &MutexOfDownload );
    this->fileids = fileids;

    this->functionselect = 1;
    this->start();
    usleep(10000);
}

void proccessdata::setheartbeatdata(Json:: Value &data)
{

    pthread_mutex_lock(&MutexOfJsondata);
    heartbeatdata = data;
    pthread_mutex_unlock(&MutexOfJsondata);

}

Json:: Value proccessdata::getheartbeatdata()
{
    Json:: Value jdata;
    pthread_mutex_lock(&MutexOfJsondata);
    jdata= heartbeatdata;
    pthread_mutex_unlock(&MutexOfJsondata);
    return jdata;
}

int proccessdata::getplaylistfile(std::string &playlist, int type)
{
    Server_Status_t status;
    status = globaflags::getInstance()->getserverstatus();

    if(type == TYPEOFPLAYLIST ) {

        pthread_mutex_lock(&MutexOfPlaylist);
        /*
        if(status.ServerStatusFlag == serveronline) {
            if(this->playlistpathandname == "NULL")
                return FAILED;
            playlist = this->playlistpathandname ;
        } else {
        */
        playlist = workfolder + DBFILESTORAGEFOLDER + "/" + DEFAULTPLAYLISTFILE;
        std::ifstream in(playlist.c_str());
        char buf[50];
        if (! in.is_open())
        { std::cout << "[proccessdata::getplaylistfile]Error opening file\n"; return FAILED; }
        memset(buf,0,50);
        in.getline(buf,50);
        playlist = buf;
        in.close();
        //     }

    } else {
        /*
        if(status.ServerStatusFlag == serveronline) {

            pthread_mutex_lock(&MutexOfAddPlaylist);
            if(this->addplaypathandname == "NULL")
                return FAILED;
            playlist = this->addplaypathandname ;
        } else {*/
        playlist = workfolder + DBFILESTORAGEFOLDER +"/" + DEFAULTADDPLAYLISTFILE;
        std::ifstream in( playlist.c_str() );
        char buf[50];
        if (! in.is_open())
        { std::cout << "[proccessdata::getplaylistfile]Error opening file\n"; return FAILED; }
        memset(buf,0,50);
        in.getline(buf,50);
        playlist = buf;
        in.close();
    }
    // }
    return SUCCESS;
}

void proccessdata::postplaylistfile ( int type )
{

    if(type == TYPEOFPLAYLIST ) {

        pthread_mutex_unlock(&MutexOfPlaylist);

    } else {

        pthread_mutex_unlock(&MutexOfAddPlaylist);

    }

}

static int playlistcheckflag = 0;


int  proccessdata::checkplaylist(std::string &workfolder)
{
    std::string cmd;
    std::string data;
    Tools tool;
    std::vector<std::string> strings;
    int lines;

    cmd = "ls " + workfolder + DBFILESTORAGEFOLDER + "/*.db3 2>&1";

    lines = tool.myexec( cmd.c_str() , strings );
    if( lines > 0 ) {

        data = strings[0];

        if(data.find("Not a directory",0 ) != -1 ) {
            cmd = "rm " +workfolder + DBFILESTORAGEFOLDER;
            system(cmd.c_str());
            cmd = "mkdir " +workfolder + DBFILESTORAGEFOLDER;
            system(cmd.c_str());
            std::cout << ">>> " << data << std::endl;
            return NOTEXIST;
        } else  if( data.find( "No such file or directory",0 ) != -1 ) {
            std::cout << ">>> " << data << std::endl;
            return NOTEXIST;
        }
    }

    return EXIST;
}

int proccessdata::getplaylist (std::string &playlistid , int type)
{
    std::string url;
    std::string filepathandname;
    std::string cmd;

    url = "http://" + serverip;
    url += HTTPGETPLAYLISTURL;
    url += playlistid;

    filepathandname = workfolder + DBFILESTORAGEFOLDER + "/playlist_" + playlistid + ".db3";

    cmd = "rm " + filepathandname + " >/dev/null 2>&1 ";

    pthread_mutex_lock(&MutexOfPlaylist);

    printf("downloading file :%s\n",filepathandname.c_str());

    system(cmd.c_str());
    pthread_mutex_lock( &MutexOfHttp);
    Client.download(url,filepathandname,1);
    pthread_mutex_unlock( &MutexOfHttp );


    if (type == TYPEOFPLAYLIST) {
        this->playlistpathandname = filepathandname;
        cmd  = workfolder + DBFILESTORAGEFOLDER + "/" + DEFAULTPLAYLISTFILE;
        std::ofstream out(cmd.c_str());
        if (! out.is_open())
        { std::cout << "[proccessdata::getplaylist]Error opening file\n"; return FAILED; }
        out << playlistpathandname;
        out.close();
    }  else {
        this->addplaypathandname = filepathandname;
        cmd  = workfolder + DBFILESTORAGEFOLDER + "/" + DEFAULTADDPLAYLISTFILE;
        std::ofstream out(cmd.c_str());
        if (! out.is_open())
        { std::cout << "[proccessdata::getplaylist]Error opening file\n"; return FAILED; }
        out << addplaypathandname;
        out.close();
    }
    pthread_mutex_unlock( &MutexOfPlaylist );

    sem_post(&globaflags::getInstance()->semofupgradeplaylist);

    return SUCCESS;

}

int proccessdata::StoragePlaylistUpdateTime(std::string &timea ,std::string &timeb)
{
    std::string path;
    path = workfolder + LOCALDUPDATETIMEFILE;
    std::ofstream out(path.c_str());
    if (! out.is_open())
    { std::cout << "[StoragePlaylistUpdateTime]Error opening file:" << LOCALDUPDATETIMEFILE <<"\n"; return FAILED; }
    out << timea +"\n";
    out << timeb +"\n";
    out.close();

    return SUCCESS;

}

int proccessdata::GetPlaylistUpdateTime(std::string &timea ,std::string &timeb)
{
    std::string path;
    char buf[50];
    path = workfolder + LOCALDUPDATETIMEFILE;
    std::ifstream in(path.c_str());
    if (! in.is_open())
    { std::cout << "[GetPlaylistUpdateTime]Error opening file:" << LOCALDUPDATETIMEFILE <<"\n"; return FAILED; }
    in.getline(buf,50);
    timea = buf;
    in.getline(buf,50);
    timeb = buf;
    in.close();

    return SUCCESS;

}


int proccessdata::playlistcheck(std::string &playlistid , std::string &timetick, int type)
{
    std::string times;
    gettime timegetfunc;
    int ret = NOTNEEDUPDATA;

    //printf("in playlist check \n");
    if( !playlistcheckflag ) {
        playlistcheckflag = 1 ;

        if( checkplaylist( workfolder ) == NOTEXIST ) {
            printf("not any playlist exist!\n");
            getplaylist( playlistid , type);

            switch(type) {
            case TYPEOFPLAYLIST: updatetickofplaylist = timetick;break;
            case TYPEOFADDPLAY: updatetickofaddplaylist = timetick;break;
            default:break;
            }

            StoragePlaylistUpdateTime(updatetickofplaylist,updatetickofaddplaylist);

        } else {
            GetPlaylistUpdateTime(updatetickofplaylist,updatetickofaddplaylist);

            if( type == TYPEOFPLAYLIST ) {
                if( timetick == updatetickofplaylist )
                    return ret;
            } else {
                if ( timetick == updatetickofaddplaylist )
                    return ret;
            }

            times = timegetfunc.gettimetickasstring();
            printf("localtime  : %s\n",times.c_str());
            printf("updatetime : %s\n",timetick.c_str());
            if( times < timetick )
                ret = NOTNEEDUPDATA;
            else {
                printf("need to update \n");
                ret = getplaylist ( playlistid , type);


                switch(type) {
                case TYPEOFPLAYLIST: updatetickofplaylist = timetick;break;
                case TYPEOFADDPLAY: updatetickofaddplaylist = timetick;break;
                default:break;
                }
                StoragePlaylistUpdateTime(updatetickofplaylist,updatetickofaddplaylist);
            }
        }
    }
    playlistcheckflag = 0;
    return ret;
}






static pthread_t  uploadlogtid = 0;


static void * uploadlogsthreadbody(void * param)
{

    middleparam a;
    std::string url;
    HttpClient Client;
    int ret;
    std::ifstream file;
    Md5sum Summd5;
    std::string md5;

    a = *((middleparam *)param);
    if(uploadlogtid > 0 )
        pthread_detach(uploadlogtid);

    url = "http://" + a.a;
    url += HTTPUPLOADURL;
    url += "&id=";
    url +=  a.b;

    Summd5.reset();
    file.open(a.filename.c_str());
    Summd5.update(file);
    file.close();
    md5=Summd5.toString();
    url += "&md5=";
    url += md5;

    ret = Client.upload ( url, a.filename );
    if( ret != SUCCESS ) {
        DEBUG1INFO ("Client.upload failed\n");
    }

    uploadlogtid = 0;

}

void proccessdata::uploadlog()
{
    int ret;
    if(uploadlogtid == 0) {
        middleparam a;
        a.a = serverip;
        a.b = deviceid;
        a.filename = globaflags::getInstance()->reallogfile;
        ret = pthread_create( &uploadlogtid , NULL , uploadlogsthreadbody , (void *)&a );
        DEBUG1INFO("\nupload log...\n");
        usleep(50000);
    }

}




static pthread_t  getnotifytid = 0;


static
void handleItemData(Json::Value &ItemData)
{
    std::string msg;

    if( ItemData.isNull() ) {

        DEBUG1INFO("ItemData is empty !\n");
        return ;
    }

    msg = ItemData["msg"].asString();
    if( msg  == "") {
        if( globaflags::getInstance()->LogUploadFlag != "1" ) {
            DEBUG1INFO ("log upload pemission denied!\n");
            return;
        }

        if( uploadlogtid == 0 ) {
            middleparam a;
            a.a = globaflags::getInstance()->serverip;
            a.b = globaflags::getInstance()->deviceid;
            a.filename = globaflags::getInstance()->reallogfile;
            pthread_create( &uploadlogtid , NULL , uploadlogsthreadbody , (void *)&a );
            DEBUG1INFO("upload log...\n");
            usleep(50000);
        }
    }
}

static void * getnotifythreadrunbody( void* p )
{
    middleparam a;
    std::string send;
    std::string url;
    std::string recv;
    Json::Value json_data;
    Json::Value json_item;
    Json::Reader reader;
    int count;
    std::string count_string;


    a = *((middleparam *)p);

    pthread_detach(getnotifytid);

    url = "http://" + a.a;
    url += HTTPGETNOTICEURL;

    send = "id=" + a.b;
    do {

        Client.Post(url,send,recv);
        DEBUG2INFO("Notify recive date :%s\n",recv.c_str());
        reader.parse(recv,json_data);

        count =  json_data["cnt"].asInt();
        DEBUG2INFO("count  %d\n",count);

        json_item = json_data["item"];

        handleItemData( json_item );


    } while(count == 10);

    getnotifytid = 0;
}


void proccessdata::getnotify()
{
    int ret;

    if(getnotifytid == 0) {
        middleparam a;
        a.a = serverip;
        a.b = deviceid;
        ret = pthread_create(&getnotifytid,NULL,getnotifythreadrunbody,(void *)&a);

        usleep(50000);
    }

}



void  proccessdata::handleudpheartbeatdata ( )
{
    Json::Value data;
    Json::FastWriter writer;
    std::string playlist;
    std::string playlistupdat;
    int flag;

    while(1)
    {
        sem_wait ( &semofprocessdata );
        data = getheartbeatdata();
        //DEBUG2INFO("processdata recive: %s\n",writer.write(data).c_str());

        playlist = data [ recvdataformate[1] ].asString();
        playlistupdat = data[ recvdataformate[2] ].asString();
        playlistcheck( playlist , playlistupdat , TYPEOFPLAYLIST );

        playlist = data [ recvdataformate[3] ].asString();
        playlistupdat = data[ recvdataformate[4] ].asString();
        playlistcheck( playlist , playlistupdat , TYPEOFADDPLAY );

        globaflags::getInstance()->LogUploadFlag = data[recvdataformate[6]].asString();

        playlist = data[recvdataformate[5]].asString();
        if( playlist == "1" )
            getnotify();

    }

}

int proccessdata::checkdownloadfilefold(std::string &fileid)
{
    std::string cmd;
    std::string data;
    Tools tool;
    std::vector<std::string> strings;
    int lines;

    cmd = "ls " + workfolder + MUSICESTORAGEFOLDER + "/" + fileid +" 2>&1";

    lines = tool.myexec(cmd.c_str(),strings);
    if( lines > 0 ) {

        data = strings[0];

        if( data.find("Not a directory",0 ) != -1 ) {
            cmd = "rm " + workfolder + MUSICESTORAGEFOLDER;
            system(cmd.c_str());
            cmd = "mkdir " + workfolder + MUSICESTORAGEFOLDER;
            system(cmd.c_str());
            std::cout << ">>> " << data << std::endl;
            return NOTEXIST;

        } else  if (data.find("No such file or directory",0 ) != -1 ) {
            std::cout << ">>> " << data << std::endl;
            return NOTEXIST;
        }
    }

    return EXIST;

}

int proccessdata::downloadonefile(std::string &url , std::string &filepathandname)
{
    int ret;
    printf("downloading file :%s\n",filepathandname.c_str());
    pthread_mutex_lock( &MutexOfHttp);
    ret = Client.download(url,filepathandname,3);
    pthread_mutex_unlock( &MutexOfHttp);
    return ret;
}


void proccessdata::handledownloadfiles()
{
    std::string urlhead ;
    std::string url;
    std::string fileid;
    std::string filenameandpath;
    //std::vector<std::string> md5sumfileid;
    std::ifstream file;
    HandleSqlite sqlite;
    Json::Value jsondata;
    std::string cmd;
    int size;
    int  count;

    urlhead =  "http://" + serverip + HTTPDOWNLOADFILEURL;
    fileid = LOCALDBFILENAME;
    sqlite.opendatabase( fileid );
    size = fileids.size();
    for (int i = 0 ; i < size  ; i++) {
        fileid = fileids[i];
        filenameandpath = workfolder + MUSICESTORAGEFOLDER + "/" + fileid;
        jsondata.clear();
        sqlite.getdatafromedb("playlist_item","fileid",fileid,jsondata);
        if( jsondata.size() > 0 ) {

            cmd = "update playlist_item set stat=1 where fileid=\"";
            cmd += fileid + "\";";
            sqlite.runsqlitecmd(cmd);


            continue;
#if 0
            Summd5.reset();
            file.open(filenameandpath.c_str());
            Summd5.update(file);
            file.close();
            filenameandpath=Summd5.toString();
            if( tools.str_campare( filenameandpath,fileid) ) continue;
            cmd = "rm ";
            cmd +=filenameandpath;
            system(cmd.c_str());
#endif
        }

        count = 3;
        do {
            filenameandpath = workfolder + MUSICESTORAGEFOLDER + "/" + fileid;

           // cmd = "rm ";
           // cmd +=filenameandpath;
           // system(cmd.c_str());

            url = urlhead + fileid;
            downloadonefile( url , filenameandpath );
            Summd5.reset();
            file.open(filenameandpath.c_str());
            Summd5.update(file);
            file.close();
            filenameandpath=Summd5.toString();
            count--;

        }
        while( !tools.str_campare( filenameandpath,fileid) && count > 0);
        if( count <=0 ) {
            printf("failed to download :%s\n",fileid.c_str());
            filenameandpath = workfolder + MUSICESTORAGEFOLDER + "/" + fileid;
            cmd = "rm ";
            cmd +=filenameandpath;
            system(cmd.c_str());

        }
    }
    sqlite.closedatabase();
    pthread_mutex_unlock( &MutexOfDownload );
}

void proccessdata::run()
{
    int function = functionselect;
    switch(function){
    case 0:handleudpheartbeatdata();break;
    case 1:handledownloadfiles();break;
    default:break;
    }
}
