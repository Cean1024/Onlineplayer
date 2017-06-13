#include "udpclient.h"

udpclient::udpclient()
{
    memset(&server, 0 ,sizeof (struct sockaddr_in));
    serverfd = -1;
    serveraddlen = sizeof(server);

}

udpclient::~udpclient()
{
    if(serverfd < 0)
        close(serverfd);
}

int udpclient::clientsend( std::string &data )
{
    if(serverfd < 0)
        return FAILED;
    int ret;
    ret =sendto( serverfd,data.c_str(),data.length(),0,
            (struct sockaddr *)&server,serveraddlen);
    return ret;

}

int udpclient::clientrecv( std::string &data )
{
    if(serverfd < 0)
        return FAILED;
    int ret;
    memset (buf,0,recvdatalength);
    ret = recvfrom(serverfd, buf , recvdatalength ,0 , (struct sockaddr *)&server,&serveraddlen);
    data=buf;

    return ret;
}

void udpclient::run()
{
    memset (buf,0,recvdatalength);
    recvfrom(serverfd, buf , recvdatalength ,0 , (struct sockaddr *)&server,&serveraddlen);
}

int udpclient::clientrecvwithtime(std::string &data ,int times)
{
    if(serverfd < 0)
        return FAILED;
    this->start();
    usleep(10000);
    times = times * 10;
    while( times-- > 0) {
        if(this->getState() == threadstatusexit ) {
            data = buf;
            return SUCCESS;
        }
        usleep(100000);
    }
    this->distroy();

    return FAILED;
}

void udpclient::closeclient( void )
{
    if(serverfd < 0) {
        close(serverfd);
        serverfd =-1;
    }
}

int udpclient::init_client (std::string &serverip , int portnumber )
{
    //struct sockaddr_in serverid ;

    if ( ( serverfd = socket ( AF_INET ,  SOCK_DGRAM , 0 ) ) < 0 ) {
        perror("socket");
        return IFAILED;
    }

    server.sin_family = AF_INET;
    //strncpy(IP_ADDR,SERVERIP,sizeof(SERVERIP));
    //PORT = portnumber;
    server.sin_addr.s_addr = inet_addr( serverip.c_str() );
    server.sin_port = htons( portnumber );

    return ISUCCESS;

}

