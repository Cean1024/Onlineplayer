#include "heartbeat.h"


/****************heartbeat config****************/
#define headstringlength 5
#define loginstringlength 2
#define heartbeatstringlength 4
#define recevdataformatelength 7
static std::string headstring[ headstringlength ] = {"serialid","tick","type" ,"crc","body"};
static std::string loginstrings[ loginstringlength ] = {"devid","key"};
static std::string heartbeattrings[ heartbeatstringlength ] = {"devid","cpu","mem","playfileid"};
static std::string recvdataformate [ recevdataformatelength ] = {"status","playlist","playlist_update","addplaylist",
                                                          "addplaylist_update","notify","debug_log"};
/************************************************/

heartbeat::heartbeat()
{
    serialNum =0;
    udpport = 0;
}

heartbeat::~heartbeat()
{

}

int heartbeat::init()
{

    this->serverip = globaflags::getInstance()->serverip;
    this->udpport  = globaflags::getInstance()->udpport;
    this->deviceid = globaflags::getInstance()->deviceid;

    this->start();


    return ISUCCESS;
}



int heartbeat::httplogin( std::string serverip)
{
    std::string url;
    std::string postdata;
    std::string status;
    HttpClient hclient;
    Json::Value root;
    Json::FastWriter writer;


    url = "http://";
    url +=serverip;
    url += HTTPLOGINURL;

    root["devid"]  = Json::Value(deviceid);
    root["type"]   = Json::Value("1");
    root["passwd"] = Json::Value(deviceid);

    hclient.Post(url,writer.write(root),postdata);

    //DEBUG2INFO("http recv: %s",postdata.c_str());
    Json::Reader reader;
    reader.parse(postdata,root,true);

    postdata = writer.write(root["status"]);
    //DEBUG1INFO(postdata.c_str());
    status = "200\n";
    if(postdata != status)
         return FAILED;

    postdata = writer.write(root["message"]);
    //DEBUG1INFO(postdata.c_str());
    status = "\"ok\"\n";
    if(postdata != status)
         return FAILED;

    return SUCCESS;

}

std::string heartbeat::gethead(int type)
{
    char buf[100];
    std::string head;

    return head;
}

std::vector<std::string> split(std::string str,std::string pattern)
{
  std::string::size_type pos;
  std::vector<std::string> result;
  std::string s;
  str+=pattern;//扩展字符串以方便操作
  int size=str.size();

  for(int i=0; i<size; i++)
  {
    pos=str.find(pattern,i);
    if(pos<size)
    {
      s=str.substr(i,pos-i);
      result.push_back(s);
      i=pos+pattern.size()-1;
    }
  }
  return result;
}

int heartbeat::ProcessLogInData (std::string data)
{

    //Json::Value root;
    std::string status;
    std::string status_ok = "200";
    std::string crc_recv;
    std::string crc_calc;

    data = data.substr( 3 , (data.length() - 6 ));
    std::vector<std::string> result = split(data,",");

    crc_recv    =   result[3].c_str();
    status      =   result[4].c_str();
    crc_calc = getcrc32toString(status);
#if 0
    if(crc_recv != crc_calc)
        return FAILED;

    if( status != status_ok )
        return FAILED;
#endif
    DEBUGLOGOFNETWORK2INFO("%s\n",data.c_str());
    return SUCCESS;
}

int heartbeat::ProcessBeartBeatData(std::string data)
{
    Json::Value root;
    std::string recvdata;
    std::string crc_recv;
    std::string crc_calc;

    data = data.substr( 3 , (data.length() - 6 ));

    std::vector<std::string> result = split(data,",");

    recvdata = result[0];

    uint num = atoi(recvdata.c_str());
    if ( num != serialNum )     //check serial number;
        return IGENOR;

    crc_recv = result[3];
    int stringssize = result.size();

    if( stringssize < recevdataformatelength + headstringlength -1) {   // check data size;
         DEBUGLOGOFNETWORK3INFO ( "error recive data size :%d  %s\n",stringssize,data.c_str() );
        return ERROR;
    }

    recvdata = "";
    for(int i = 0 ; i < recevdataformatelength ; i++) {
        root[ recvdataformate[ i ] ] = result[ i + headstringlength - 1 ].c_str();
        recvdata += result[ i + headstringlength - 1 ];
        if ( i < recevdataformatelength - 1 ) recvdata += ",";
    }

    //DEBUGLOGOFNETWORK2INFO("recv data : %s\n",recvdata.c_str());
    crc_calc = getcrc32toString ( recvdata );

    if( crc_recv != crc_calc )
        return FAILED;

    //DEBUGLOGOFNETWORK2INFO("recv data : %s\n",recvdata.c_str());

    //DEBUGLOGOFNETWORK2INFO("%s\n",data.c_str());

    crc_calc = root["status"].asString();
    if( crc_calc != "1" )
        return FAILED;

    proccessdata::getInstance()->setheartbeatdata(root);
    sem_post(&proccessdata::getInstance()->semofprocessdata);

    return SUCCESS;
}


int heartbeat::LogIn()
{

    std::string data;
    //unsigned short crc16;
    ulong crc;
    Json::Value root;

    memset(buf,0 ,tmpbuflength);
    sprintf(buf,"%lu",++serialNum);
    root[ headstring[0].c_str() ]     =   Json::Value(buf);

    memset(buf,0 ,tmpbuflength);
    sprintf(buf,"%lu",time(0));
    root[headstring[1].c_str()]   =   Json::Value(buf);

    memset(buf,0 ,tmpbuflength);
    sprintf(buf,"%d",TYPE_LOGIN);
    root[headstring[2].c_str()]   =   Json::Value(buf);


    Json::Value body;
    body[ loginstrings[0].c_str() ]  = Json::Value ( deviceid.c_str() );

    body[ loginstrings[1].c_str() ]  = Json::Value ( deviceid.c_str() );

    data ="";
    std::string tmp;
    int i ;
    for(i = 0 ;i < loginstringlength ; i ++)
    {
        tmp = body[loginstrings[i].c_str()].asString();
        data += tmp;
        if(i < loginstringlength - 1) data += ",";
    }

    //DEBUG1INFO(data.c_str());

    //crc16 = getcrc16(data);
    crc = getcrc32(data);
    memset(buf,0,tmpbuflength);
    sprintf(buf,"%lu",crc);
    //sprintf(buf,"%u",crc16);
    root[headstring[3].c_str()]    =    Json::Value ( buf );

    root[headstring[4].c_str()]    =    Json::Value(data.c_str());


    handljsontostring(root,data);

    DEBUGLOGOFNETWORK2INFO("%s\n",data.c_str());
    udpcomminicate.clientsend( data );

    if ( udpcomminicate.clientrecvwithtime ( data , LOGINWAITTIME ) == SUCCESS ) {

        if ( ProcessLogInData(data) == SUCCESS) {

            status.ServerStatusFlag = serveronline;
            status.ServerLoginTime  = time(0);
            status.heartbeatTime    = status.ServerLoginTime;
            globaflags::getInstance()->setserverstatus(status);
            DEBUG1INFO("LOGIN : success!\n");

            return SUCCESS;
        } else {
            status.ServerStatusFlag = serveroffline;
            globaflags::getInstance()->setserverstatus(status);
            DEBUG1INFO("LOGIN : data crc error!\n");
            return FAILED;
        }
    }

    status.ServerStatusFlag = serveroffline;
    globaflags::getInstance()->setserverstatus(status);
    DEBUG1INFO("LOGIN waiting timeout  \n");


    return FAILED;

}




Json::Value heartbeat::GetBeatPackage()
{
    std::string data;
    ulong crc32;
    cpuuserate = cpuandmem_d.getcpuusedrate();
    cpuandmem_d.getfreemem(mem);

    memset(buf,0 ,tmpbuflength);
    sprintf(buf,"%lu",++serialNum);
    root[ headstring[0].c_str() ]     =   Json::Value(buf);

    memset(buf,0 ,tmpbuflength);
    sprintf(buf,"%lu",time(0));
    root[headstring[1].c_str()]   =   Json::Value(buf);

    memset(buf,0 ,tmpbuflength);
    sprintf(buf,"%d",TYPE_HEARTBEAT);
    root[headstring[2].c_str()]   =   Json::Value(buf);


    Json::Value body;
    body[ heartbeattrings[0].c_str() ]  = Json::Value ( deviceid.c_str() );

    memset(buf,0 ,tmpbuflength);
    sprintf( buf,"%d",cpuuserate  );
    body[ heartbeattrings[1].c_str() ]    = Json::Value ( buf );

    if( mem.free < 500 )
    {
        std::string info;
        sprintf(buf,"%lu",mem.free);
        //info = Time.gettimetickasstring() + " 设备异常 内存不足，当前内存" +buf ;
        info = "[" + Time.getdateasstring()+" " +Time.gettimeasstring() + "]  设备异常 内存不足，当前内存" +buf ;
        sprintf(buf,"%lu",mem.total);
        info = info + "/" + buf;
        LOG_STORAGE (info , globaflags::getInstance()->reallogfile );
    }

    memset(buf,0 ,tmpbuflength);
    sprintf(buf,"%lu",mem.free);
    body[ heartbeattrings[2].c_str() ]    = Json::Value ( buf );


    std::string fileid;
    globaflags::getInstance()->outputcurrentfileid(fileid);
    body[ heartbeattrings[3].c_str() ] = Json::Value ( fileid.c_str() );

    data ="";
    std::string tmp;
    int i ;
    for(i = 0 ;i < heartbeatstringlength ; i ++)
    {
        tmp = body[heartbeattrings[i].c_str()].asString();
        data += tmp;
        if(i < heartbeatstringlength - 1) data += ",";
    }

    crc32 = getcrc32( data );
    memset(buf,0,tmpbuflength);
    sprintf(buf,"%lu",crc32);
    root[headstring[3].c_str()]    =    Json::Value ( buf );

    root[headstring[4].c_str()]    =    Json::Value(data.c_str());

    //data = writer.write(root);
    return root;
}

void heartbeat::handljsontostring(Json::Value &root , std::string &data)
{
    data="<*<";

    for(int i = 0 ;i < headstringlength ; i ++)
    {
        data += root[headstring[i].c_str()].asString();

        if(i < headstringlength - 1) data += ",";
    }
    data += ">*>";
}

static Json::Value root;

int heartbeat::BeartBeat()
{

    //Json::FastWriter writer;
    std::string data;

    //std::cout<<data<<std::endl;

    root = GetBeatPackage();

    handljsontostring(root,data);

    //DEBUGLOGOFNETWORK2INFO("send : %s\n",data.c_str());
    udpcomminicate.clientsend( data );

    if ( udpcomminicate.clientrecvwithtime ( data , LOGINWAITTIME ) == SUCCESS ) {
        //DEBUG2INFO("recv : %s\n", data.c_str());

        if ( ProcessBeartBeatData(data) == SUCCESS ) {

            status.ServerStatusFlag = serveronline;
            status.ServerLoginTime  = time(0);
            status.heartbeatTime    = status.ServerLoginTime;
            globaflags::getInstance()->setserverstatus(status);

            return SUCCESS;
        }
    }

    return FAILED;
}

#ifdef CRC_16
ushort heartbeat::getcrc16(std::string Data)
{
    ushort crc = 0;
    const char * buf = Data.c_str();
    int length = Data.length();
    while (length-- > 0)
        crc = ccitt_table[(crc >> 8 ^ *buf++) & 0xff] ^ (crc << 8);
    return crc;
}
#endif

ulong heartbeat::getcrc32(std::string data)
{

    const char * buf = data.c_str();
    int length = data.length();
    ulong crcval;

    /*
     uLong crc = crc32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       crc = crc32(crc, buffer, length);
     }
     if (crc != original_crc) error();
     */
    crcval = crc32(0L,(const Bytef *)buf,length);
    return crcval;

}
std::string heartbeat::getcrc32toString(std::string data)
{

    const char * buf = data.c_str();
    int length = data.length();
    ulong crcval;
    char tmpbuf[50];

    /*
     uLong crc = crc32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       crc = crc32(crc, buffer, length);
     }
     if (crc != original_crc) error();
     */
    crcval = crc32(0L,(const Bytef *)buf,length);
    memset(tmpbuf,0,50);
    sprintf(tmpbuf,"%lu",crcval);
    data = tmpbuf;

    return data;

}


int check_network()
{
    Tools myexec;
    std::vector<std::string> status;

    myexec.myexec("cat /proc/net/route | grep eth0",status);
    if(status.size() > 0)
        return SUCCESS;

    return FAILED;

}


static int logflagwaitnetwork = 0;
static int logflaghttplogin = 0;

void heartbeat::run( )
{
    Server_Status_t t_status;

    std::string info;

    while(1) {

        //sleep(1);
        if( check_network() == FAILED ) {

            if(logflagwaitnetwork == 0) {
                logflagwaitnetwork = 1;
                DEBUG1INFO ("waiting network ... \n");
                //info = Time.gettimetickasstring() + " 设备异常 网络中断";
                info = "[" + Time.getdateasstring()+" " +Time.gettimeasstring() + "] 设备异常 网络中断";
                LOG_STORAGE (info , globaflags::getInstance()->reallogfile );
            }
            sleep(2);
            //
        } else {
            logflagwaitnetwork = 0;
            //DEBUG1INFO("network is ok \n");

            if( httplogin(serverip) == FAILED ) {
                //DEBUG1INFO("http login error! check device id and key \n");
                if(logflaghttplogin == 0) {
                    logflaghttplogin = 1;
                    //info = Time.gettimetickasstring() + " 设备异常 http登陆失败";
                    info = "[" + Time.getdateasstring()+" " +Time.gettimeasstring() + "] 设备异常 http登陆失败";
                    LOG_STORAGE (info , globaflags::getInstance()->reallogfile );
                }
                sleep(5);
                continue;
            }
            logflaghttplogin = 0;
            if( udpcomminicate.init_client(serverip,udpport) == IFAILED) {
                 DEBUG1INFO("init udp error!");
                 return ;
             }
            if( LogIn() == FAILED ) {
                DEBUG1INFO("login error! check device id and key \n");
                udpcomminicate.closeclient();
                sleep(5);
                continue;
            }

            //info = Time.gettimetickasstring() + " 网络正常 登陆成功";
            info = "[" + Time.getdateasstring()+" " +Time.gettimeasstring() + "] 网络正常 登陆成功";
            LOG_STORAGE (info , globaflags::getInstance()->reallogfile );


            while(1) {

                if( BeartBeat() == SUCCESS ) {
                    sleep(4);
                } else {
                    status.heartbeatTime    = time(0);
                    t_status = globaflags::getInstance()->getserverstatus();
                    if(status.heartbeatTime - t_status.heartbeatTime > INTERVALTIME)
                    {
                       DEBUG1INFO("cannot connect to server\n");
                       udpcomminicate.closeclient();
                       break;
                    }
                }

            }
        }
    }
}

