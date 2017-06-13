#include "update.h"

static Tools tool;
static vector<string> resoult;

Update::Update()
{

}

Update::~Update()
{

}

r_status Update::init()
{

    this->start();

    return SUCCESS;
}

r_status Update::handlerecivedate(Netlink &dataNL , std::string &recv , hotplugstat &status)
{

    int index;
    while(1) {

        dataNL.recvfrome(recv);
        if( recv.find("add@",0) != recv.npos ) {
            if( (index = recv.find("sd",0)) == recv.npos  ) continue;
            recv = recv.substr(index , recv.length() - index);
            if( (index = recv.find("/",0)) == recv.npos  ) continue;

            recv = recv.substr( index + 1 , recv.length() - index );
            std::cout << "add " << recv <<std::endl;
            status = ADDDEVICE;
            sleep(2);
            return SUCCESS;

        } else if (recv.find("remove@",0) != recv.npos ) {

            if( (index = recv.find("sd",0)) == recv.npos  ) continue;
            recv = recv.substr(index , recv.length() - index);
            if( (index = recv.find("/",0)) == recv.npos  ) continue;

            recv = recv.substr( index + 1 , recv.length() - index );
            std::cout<< "remove "  << recv <<std::endl;
            status = REMOVEDEVICE;
            return SUCCESS;

        }

    }
}

static r_status CheckUpdatePackage( std::string &UdiskFolder )
{
    string cmd;

    cmd = UdiskFolder;
    struct dirent    *dp;
    DIR              *dfd;

    cmd = cmd.substr(1,cmd.length());
    if( (dfd = opendir(cmd.c_str())) == NULL )
    {
        printf("open dir failed! dir:%s\n", UdiskFolder.c_str());
        return FAILED;
    }
    for(dp = readdir(dfd); NULL != dp; dp = readdir(dfd))
    {
        if( strstr( dp->d_name,"onlineplayer_V" )!=NULL )
        {
            if( strstr( dp->d_name,".tar.gz" )!=NULL  )
            {

                printf( "udisk tars:[ %s ]\n",dp->d_name );
                cmd = "/root/onlineplayer/softupdate ";
                cmd += UdiskFolder;
                printf( "cmd:%s\n" , cmd.c_str() );
                system( cmd.c_str() );
               #if 0
                cmd = "cp ";
                cmd += dp->d_name;
                cmd += " /run";
                printf( "cmd:%s\n",cmd.c_str() );
                system(cmd.c_str());
                cmd = "/root/onlineplayer/softupdate /run";
                printf( "cmd:%s\n",cmd.c_str() );
                system(cmd.c_str());
             #endif

            }

        }

    }

    closedir(dfd);

    return SUCCESS;

    //tool.myexec();

}

static void * userthreadbody( void * param )
{
    pthread_detach(pthread_self());

    using namespace std;
    string cmd;
    int ret , count ;
    vector<string> resoult;

    char *recv = (char *) param;

    cmd = "df -h | grep ";
    cmd += recv ;
    cmd += " | cut -d '%' -f 2 2>&1";
    //cout << cmd << endl;
    count = DFWAITECOUNT ;
    ret = 0;
    do{
        sleep(1);
        ret = tool.myexec ( cmd,resoult );
    } while ( ret <= 0 && --count > 0);

    if(ret <= 0 && count == 0 ) {
        printf("myexec error! ret :%d count :%d\n",ret,count);
        return NULL;
    }
    if( globaflags::getInstance()->getplatformflag() != platformisi386 )
        CheckUpdatePackage( resoult[0] );

    cout << resoult[0] << endl;

}

void Update::HandleAddDevice(std::string &recv)
{
    int ret;
    pthread_t userthreadbodyid;
    ret = pthread_create( &userthreadbodyid , NULL , userthreadbody , (void *)(recv.c_str()));
    DEBUG1INFO("\HandleAddDevice thread runing...\n");
    usleep(50000);

}

void Update::HandleAddRemove(std::string &recv , hotplugstat &status)
{

    switch(status) {
    case ADDDEVICE: {

        HandleAddDevice(recv);

    };break;
    case REMOVEDEVICE: {

        if( globaflags::getInstance()->getplatformflag() == platformisi386 ) return;

        using namespace std;
        string cmd;


        cmd = "df -h | grep ";
        cmd += recv + " | cut -d '%' -f 2 2>&1";
        //cout << cmd << endl;


        if ( tool.myexec ( cmd , resoult ) <= 0 ) {
            printf( "myexec error!\n" );
            return;
        }
        //cout << resoult[0] << endl;
        cmd = "umount ";
        cmd += resoult[0];
        cout << cmd << endl;
        tool.myexec(cmd,resoult);

    };break;
    default:break;
    }
}

void Update::run()
{
    r_status ret;
    std::string recv;
    hotplugstat status;

    printf("update runing!\n");
    ret = dataNL.init_client(getThreadID());
    if(ret == FAILED) {
        printf("netlink init failed!\n");
        return;
    }
    //printf("update recving!\n");
    while(1) {

        handlerecivedate(dataNL , recv ,status);

        HandleAddRemove(recv,status);
        //printf("[%s]\n",recv.c_str());
    }
}

