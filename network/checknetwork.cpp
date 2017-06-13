#include "checknetwork.h"

checknetwork::checknetwork()
{

}

checknetwork::~checknetwork()
{

}
int checknetwork::init()
{
    pthread_mutex_init( &NetworkStatusFlag , NULL);
    NetworkStatus = networkoffline;
    this->start();

    return SUCCESS;
}
int checknetwork::getnetworkstatus()
{
    int tmp;
    pthread_mutex_lock(&NetworkStatusFlag);
    tmp = NetworkStatus;
    pthread_mutex_unlock(&NetworkStatusFlag);
    return tmp;
}

void checknetwork::setnetworkstatus(statusofnetwork status)
{
    pthread_mutex_lock(&NetworkStatusFlag);
    NetworkStatus = status;
    pthread_mutex_unlock(&NetworkStatusFlag);
}

void checknetwork::run()
{
    FILE * filefd;
    char filebuf[100];
    //audiothreadcheck check;
    //check.start();
    while(1)
    {
        filefd = popen("cat /proc/net/route | grep eth0","r");
        if(filefd == NULL){
            perror("popen");
            return ;

        }
        fgets(filebuf,100,filefd);
        pclose(filefd);
        if(strstr(filebuf,"eth0") == NULL) {
            pthread_mutex_lock(&NetworkStatusFlag);
            NetworkStatus = networkoffline;
            pthread_mutex_unlock(&NetworkStatusFlag);
            std::cout << "network offline " <<std::endl;

        } else {
            pthread_mutex_lock(&NetworkStatusFlag);
            NetworkStatus = networkonline;
            pthread_mutex_unlock(&NetworkStatusFlag);
            std::cout << "network online " <<std::endl;
        }

        sleep(5);
    }
}
