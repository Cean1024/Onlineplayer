#include "netclient.h"

netclient::netclient()
{

}

netclient::~netclient()
{

}

void netclient::getserverip(char * serverip,int size)
{
    printf("input serverip:");
    fgets(serverip,size,stdin);

}
int netclient::init_client(struct sockaddr_in *server,int *clientfd)
{
    //struct sockaddr_in serverid;

    if  (( *clientfd = socket ( AF_INET ,  SOCK_STREAM , 0 ) )< 0 ) {
        perror("LISTEN");
        return IFAILED;
    }
    memset(server, 0 ,sizeof (struct sockaddr_in));
    server->sin_family = AF_INET;
    strncpy(IP_ADDR,SERVERIP,sizeof(SERVERIP));
    PORT = PortNumber;
    server->sin_addr.s_addr = inet_addr( IP_ADDR );
    server->sin_port = htons( PORT );

    return ISUCCESS;

}

int netclient::init(cachebuf *buf)
{
    //this->countbyte = audioalsa::getInstance().param->countbytes;
    this->bufsize   = buf->getobuffsize();
    this->audiocachebuf = buf;
    this->start();
    return ISUCCESS;
}
int netclient::dealwavdata(char * databuf, unsigned int size)
{
    //sem_wait(&connnetwork::getInstance().prepair);
    //pthread_mutex_lock(&connnetwork::getInstance().bufmatrux);
    memcpy(audiocachebuf->obuff,databuf ,size );
    audiocachebuf->setobuffrecivesize(size);
    //sem_post(&connnetwork::getInstance().ready);
    //pthread_mutex_unlock(&connnetwork::getInstance().bufmatrux);
}

int netclient::buffinit()
{
    tmp = (char *)&buf;
    audiobuf=(audiodata_t * )malloc(sizeof(audiodata_t));
    if( audiobuf==NULL ){
        perror("malloc");
        return IFAILED;
    }
    audiobufintsize = ( sizeof (audiodata_t) - sizeof (audiobuf->data));
    printf("audiobufintsize :%d\n",audiobufintsize);
    tmpbufsize = bufsize + audiobufintsize;
    audiobuf->data = (char *)malloc( tmpbufsize );
    tmpbuf = audiobuf->data;
    if( audiobuf->data == NULL ){
        perror("malloc data ");
        return IFAILED;
    }
    return ISUCCESS;
}

void netclient::dealaudiodata()
{
    buf.header.cmd_type = REQ_AUDIODATA;
    buf.header.ret_code = bufsize;
    printf("bufsize :%d\n",bufsize);
    if( send(clientfd , tmp , sizeof (xprotocol_t) , 0) <= 0) {
        perror("send");
        return;
    }
    printf("tmpbufsize:%d\n",tmpbufsize);
    while(1) {
        ret = recv( clientfd, tmpbuf , tmpbufsize ,0);
        if(ret <= 0) {
            perror("recv2 ");
            return ;
        }

        audiobuf->flag  = *((int *)tmpbuf);
        audiobuf->size  = *((unsigned int *)(tmpbuf + sizeof(unsigned int)));
        audiobuf->type  = *((int *)(tmpbuf + sizeof(unsigned int) + sizeof(int)));
        databuf = tmpbuf + audiobufintsize;
        if(audiobuf->size <= 0) {
            //printf("recived bufsize :%d\n",audiobuf->size);
            continue;
        }
        if(audiobuf->size < bufsize) {
            printf("recived bufsize :%d\n",audiobuf->size);
         //   continue;
        }

        if(audiobuf->type == audio_wav ) {
            if(audiobuf->flag == REQ_DATA) {
                dealwavdata ( databuf, audiobuf->size);
                //dealwavdata ( databuf, bufsize);
            } else if (audiobuf->flag == REQ_HEADER ) {
                printf("head come in\n");
                dealwavdata ( databuf + 0x58 , audiobuf->size - 0x58 );
            } else {
                printf("end come in\n");
            }
        } else if (audiobuf->type == audio_mp3)  {
            printf("flag : %d \n",audiobuf->flag );
            printf("size : %d \n",audiobuf->size );
            printf("type : %d \n",audiobuf->type );
        } else {
            dealwavdata ( databuf, bufsize);
        }
    }
}

void netclient::run()
{
    ret = init_client(&server_addr,&clientfd);
    if(ret == IFAILED)
        return;

    ret = buffinit();
    if(ret == IFAILED)
        return;

    while(1) {
        if ( ( connect ( clientfd ,(struct sockaddr *)&server_addr ,
                         sizeof (struct sockaddr_in) ) ) < 0) {
            perror("connect");
            return;
        }
        send(clientfd , tmp , sizeof (xprotocol_t) , 0);
        ret = recv( clientfd, tmp, sizeof (xprotocol_t) ,0);
        if ( ret  <= 0 ) {
            perror("recv1");
            return;
        }
        if(buf.header.cmd_type == RPL_AUDIODATA)
        {
            dealaudiodata();
        }
    }
}
