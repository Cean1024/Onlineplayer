#ifndef NETLINK_H
#define NETLINK_H
#include <iostream>
#include <linux/netlink.h>
#include <sys/socket.h>
#include "globaflags.h"

using namespace std;
#define NETLINKBUFFSIZE 4096

class Netlink
{
public:
    Netlink();
    ~Netlink();
    r_status  init_client(pthread_t threadid);
    r_status recvfrome(string &recv);

private:
    int socket_fd;
    struct sockaddr_nl sockaddr_nl_d ;
    struct msghdr msg;
    struct iovec iov;
    char buf[NETLINKBUFFSIZE];


};

#endif // NETLINK_H
