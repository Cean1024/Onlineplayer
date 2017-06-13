#ifndef UDPCLIENT_H
#define UDPCLIENT_H
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "globaflags.h"

#define recvdatalength 1024
class udpclient:public Thread
{
public:
    udpclient();
    ~udpclient();
    int init_client( std::string &serverip , int portnumber);
    int clientsend( std::string &data );
    int clientrecv( std::string &data );
    int clientrecvwithtime(std::string &data ,int times);
    void closeclient(void);

protected:
    void run();


private:
    struct sockaddr_in server;
    socklen_t serveraddlen;
    int serverfd;
    char buf[recvdatalength];
    std::string recvdata;

};

#endif // UDPCLIENT_H
