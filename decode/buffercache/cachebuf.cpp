#include "cachebuf.h"

cachebuf::cachebuf()
{
    cache = NULL;
    headp = 0;
    outputp = 0;
    inputp = 0;
    blocksize = 0;
    maxcachsize = 0;
    readflag = 0;
    writeflag = 0;
    mbuff = NULL;
    pbuff = NULL;
    obuff = NULL;
}

cachebuf::~cachebuf()
{
    //int i = CACHEBLOCKCOUNT;
    //while( i > 0)
    //    free(cache[--i].data);
    //free(cache);

    if(mbuff)
        free(mbuff);
    if(pbuff)
        free(pbuff);
    if(obuff)
        free(obuff);
}

unsigned int cachebuf::getmaxcachsize()
{
    return maxcachsize;
}
int cachebuf::getobuffsize()
{
    return obuffsize;
}

int cachebuf::setobuffrecivesize(unsigned int size)
{
    if( size < 0) return FAILED;
    this->obuffrecivesize =size;
    return SUCCESS;
}

int cachebuf::setmbuffrecivesize(unsigned int size)
{
    if( size < 0) return FAILED;
    this->mbuffrecivesize =size;
    return SUCCESS;

}
int cachebuf::setpbuffrecivesize(unsigned int size)
{
    if( size < 0) return FAILED;
    this->pbuffrecivesize =size;
    return SUCCESS;

}

unsigned int  cachebuf::getobuffrecivesize()
{
    return this->obuffrecivesize;
}
unsigned int  cachebuf::getmbuffrecivesize()
{
    return this->mbuffrecivesize;
}
unsigned int  cachebuf::getpbuffrecivesize()
{
    return this->pbuffrecivesize;
}



 int cachebuf::destroycache()
{
    //int i = CACHEBLOCKCOUNT;
    //while( i > 0)
    //    free(cache[--i].data);
    //free(cache);

    if(mbuff)
        free(mbuff);
    if(pbuff)
        free(pbuff);
    if(obuff)
        free(obuff);
    //cache = NULL;
    headp = 0;
    outputp = 0;
    inputp = 0;
    blocksize = 0;
    maxcachsize = 0;
    readflag = 0;
    writeflag = 0;
    mbuff = NULL;
    pbuff = NULL;
    obuff = NULL;
}

int cachebuf::createcache()
{
    mbuff = (char *)malloc( maxcachsize );
    if( mbuff == NULL ) {
        return IFAILED;
    }
    pbuff = (char *)malloc( maxcachsize );
    if( pbuff == NULL ) {
        free(mbuff);
        return IFAILED;
    }
    obuff = (char *)malloc( obuffsize );
    if( pbuff == NULL ) {
        free(mbuff);
        free(pbuff);
        return IFAILED;
    }

#if 0
    int i,count =CACHEBLOCKCOUNT;
    cache = (datablock_t *)malloc( sizeof(datablock_t) * CACHEBLOCKCOUNT );
    if(cache == NULL)
        return IFAILED;

    for(i = 0; i <count;i++)
    {
        cache[i].size = CACHEBLOCKSiZE;
        cache[i].datasize = 0;
        cache[i].data = (char *)malloc( CACHEBLOCKSiZE );
        if( cache[i].data == NULL )
        {
            while( i > 0)
                free(cache[--i].data);
            return IFAILED;
        }
        memset(cache[i].data,'0',cache[i].size);
    }
#endif
    return ISUCCESS;
}

#if 0
bool cachebuf::checkmpty()
{
    bool ret;
    if( outputp == inputp && headp <= 0 )
        ret = true;
    else
        ret =false;
    return ret;
}
bool cachebuf::checkfull()
{
    bool ret;
    if( outputp == inputp && headp > 0 )
        ret = true;
    else
        ret =false;
    return ret;
}

int cachebuf::putdata(char * buf, int size, calltype type)
{

    int offset=0;
    if(writeflag > 0) return FAILED;
        writeflag++;
    if(size % blocksize != 0)
    {
        printf("data size %d not align! requite %d align!\n",size,blocksize);
        writeflag--;
        return FAILED;
    }

    while(size > 0 ){

        if(inputp >= maxcachsize ) {
            inputp = 0;
            headp++;
        }

        if(type == callwait )
            while(checkfull()) usleep(10);
        else
            if(checkfull()) {
            writeflag--;
                return FAILED;
            }

        // if(size > blocksize){
        buf += offset;
        cache[inputp].datasize = blocksize;
        strncpy( cache[inputp++].data, buf,blocksize );
        size -= blocksize;
        offset = blocksize;
        //} else {
        //  strncpy( cache[inputp++].data, buf,size);
        //  cache[inputp++].datasize = size;
        //  size -=size;
        // }
    }
    writeflag--;

    return SUCCESS;
}

int cachebuf::getdata(char *buf,int size,calltype type)
{
    int offset=0;
    if(readflag >0) return FAILED;
    readflag++;
#if 0
    strncpy(buf,cache[outputp].data,cache[outputp].datasize);
    cache[outputp].datasize = 0;
    outputp++;
    readflag--;
    if(outputp >= maxcachsize ) {
        outputp = 0;
        headp--;
    }
    return SUCCESS;

#endif
    if(size % blocksize != 0)
    {
        printf("data size %d not align! requite %d align!\n",size,blocksize);
        readflag--;
        return FAILED;
    }
    while( size > 0 )
    {
        if(outputp >= maxcachsize ) {
            outputp = 0;
            headp--;
        }

        if(type == callwait )
            while(checkmpty()) usleep(1);
        else
            if(checkmpty()) {
                readflag--;
                return FAILED;
            }

        buf += offset;
        cache[outputp].datasize = 0;
        strncpy(buf,cache[outputp++].data,blocksize);
        size -=blocksize;
    }
    readflag--;

    return SUCCESS;
}

#endif

bool cachebuf::init(int byte)
{
    int ret;
    if(byte < 0 || byte > 10) {
        printf("illigal cache init param : byte %d !\n",byte);
        return false;
    }
    countbyte = byte;
    //blocksize = CACHEBLOCKSiZE;
    maxcachsize = CACHEBLOCKSiZE * CACHEBLOCKCOUNT * countbyte;  // BUFFSIZE = CACHEBLOCKSiZE
    obuffsize   = BUFFSIZE * MAXLEN_AUDIODATACOUNT * countbyte;
    ret = createcache();
    if(ret == IFAILED)
        return false;
    return true;
}
