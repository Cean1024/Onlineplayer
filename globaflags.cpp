#include "globaflags.h"

globaflags::globaflags()
{
    Serverstatus.ServerStatusFlag=serveroffline;
    Serverstatus.ServerLoginTime = 0;
    Serverstatus.heartbeatTime = 0;
    pthread_mutex_init(&MutexOfServer,NULL);
    pthread_mutex_init(&MutexOfFileid,NULL);

    sem_init(&semofupgradeplaylist,0,0);

    BoardPcFlag = -1;

}

globaflags::~globaflags()
{

}


int globaflags::init(char * workfolder)
{
    char buf[100];
    char *p;


    char *tok=":\n";

    if( workfolder)
        this->workfolder = workfolder;

    this->reallogfile = this->workfolder + LOGFILE;
    std::string tmp = this->workfolder + DEVICEIDFOLDER;
    std::ifstream in( tmp.c_str() );

    if ( !in.is_open() )
    { std::cout << "[globaflags init]Error opening file " << tmp ; return NULL; }

    in.getline(buf,100);
    p = strtok( buf , tok );
    p = strtok( NULL ,tok);
    deviceid = p;

    in.getline(buf,100);
    p = strtok( buf , tok );
    p = strtok( NULL ,tok);
    serverip = p;

    in.getline(buf,100);
    p = strtok( buf , tok );
    p = strtok( NULL ,tok);
    sscanf(p , "%d" , &udpport);

    in.getline(buf,100);
    p = strtok( buf , tok );
    p = strtok( NULL ,tok);
    sscanf(p , "%d" , &tcpport);

    in.close();


    FILE *FILEFP = popen("who","r");
    fgets(buf,50,FILEFP);
    if( strstr(buf,"cean") != NULL ) {
        BoardPcFlag = platformisi386 ;
        DEBUG1INFO("i386 platform\n");
    } else {
        BoardPcFlag = platformisrespberry;
        DEBUG1INFO("raspberry platform\n");
    }

    fclose(FILEFP);

    DEBUG2INFO("devid :%s\n",deviceid.c_str()) ;
    DEBUG2INFO("serverip :%s\n" ,serverip.c_str());
    DEBUG2INFO("udpport :%d\n" ,udpport );
    DEBUG2INFO("tcpport :%d\n" ,tcpport );

    std::string cmd;
    cmd = "mkdir " + this->workfolder + MUSICESTORAGEFOLDER + " > /dev/null 2>&1";
    system(cmd.c_str());
    cmd = "mkdir " + this->workfolder + DBFILESTORAGEFOLDER + " > /dev/null 2>&1";
    system(cmd.c_str());

}



int globaflags::getplatformflag()
{
    return BoardPcFlag;
}

void globaflags::setserverstatus( Server_Status_t &status )
{
    pthread_mutex_lock(&MutexOfServer);
    Serverstatus = status;
    pthread_mutex_unlock(&MutexOfServer);
}

Server_Status_t globaflags::getserverstatus()
{
    Server_Status_t status;
    pthread_mutex_lock(&MutexOfServer);
    status = Serverstatus ;
    pthread_mutex_unlock(&MutexOfServer);
    return status;
}


void globaflags::inputcurrentfileid(std::string fileid)
{
    pthread_mutex_lock(&MutexOfFileid);

    this->fileid = fileid;

    pthread_mutex_unlock(&MutexOfFileid);

}

int globaflags::outputcurrentfileid(std::string &fileid)
{
    pthread_mutex_lock(&MutexOfFileid);

    fileid = this->fileid ;

    pthread_mutex_unlock(&MutexOfFileid);
    return SUCCESS;
}
