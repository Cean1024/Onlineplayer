#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "thread.h"
#include "globaflags.h"
#include "network/protocal.h"
//#include "network/connnetwork.h"
#include "decode/buffercache/cachebuf.h"
//#include ""
#define PortNumber 9009
//#define SERVERIP "192.168.1.109"
#define SERVERIP "10.10.6.209"


class netclient:public Thread
{
public:
    netclient();
    ~netclient();
    static netclient &getInstance(){
     static netclient Netclient;
     return Netclient;
    }
    audiodata_t * audiobuf;
    int init(cachebuf *buf);

protected:
    void run();
    void getserverip(char * serverip,int size);
    int init_client(struct sockaddr_in *server, int *clientfd);
    void dealaudiodata();
    int dealwavdata(char *databuf, unsigned int size);
    int buffinit();



private:
    int ret;
    int sockfd;
    char buffer[1024];
    int clientfd;
    char  IP_ADDR[16];
    int PORT;
    int countbyte;
    int bufsize;

    char * tmp;
    char * tmpbuf;
    char * databuf;
    int tmpbufsize;
    int audiobufintsize;

    xprotocol_t buf;
    cachebuf *audiocachebuf;
    struct sockaddr_in server_addr;
    struct hostent * host;
    //char serverip[20];

};

#endif // NETCLIENT_H
