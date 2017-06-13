#include "netlink.h"

Netlink::Netlink()
{
    socket_fd =-1;
}

Netlink::~Netlink()
{

}

r_status Netlink::init_client(pthread_t threadid)
{

    socket_fd = socket ( AF_NETLINK , SOCK_RAW , NETLINK_KOBJECT_UEVENT );
    if ( socket_fd < 0 ) {
        perror("socket");
        return FAILED;
    }
    sockaddr_nl_d.nl_family = AF_NETLINK;
    sockaddr_nl_d.nl_pad = 0;
    sockaddr_nl_d.nl_pid = threadid << 16 | getpid();
    sockaddr_nl_d.nl_groups = NETLINK_KOBJECT_UEVENT;
    if ( bind(socket_fd, (struct sockaddr*)&sockaddr_nl_d, sizeof(struct sockaddr_nl)) < 0)
        return FAILED;

    iov.iov_base = (void *)buf;
    iov.iov_len = NETLINKBUFFSIZE;
    msg.msg_name = (void *)&(sockaddr_nl_d);
    msg.msg_namelen = sizeof(sockaddr_nl_d);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    return SUCCESS;

}
r_status Netlink::recvfrome (string &recv)
{
    recvmsg( socket_fd , &msg , 0);
    recv=buf;
    return SUCCESS;
}




