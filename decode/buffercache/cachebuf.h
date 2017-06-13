#ifndef CACHEBUF_H
#define CACHEBUF_H


#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "globaflags.h"
//#include "decode/audio.h"


#define CACHEBLOCKCOUNT  ( MAXLEN_AUDIODATACOUNT * 16 )  // cache buff size  byte

//#define CACHEBLOCKSiZE ( 1 << 11)
#define CACHEBLOCKSiZE ( BUFFSIZE )

typedef struct datablock {
    int size;
    int datasize;
    char *data;
}datablock_t;

/*
typedef struct datablock {
    int size;
    int datasize;
    char *data;
}datablock_t;
*/
class cachebuf
{
public:
    cachebuf();
    ~cachebuf();

    bool init(int byte);
    int destroycache();
    int putdata(char *buf, int size, calltype type);
    int getdata(char * buf,int size,calltype type);
    unsigned int getmaxcachsize();
    int getobuffsize();
    unsigned int getobuffrecivesize();
    unsigned int getmbuffrecivesize();
    unsigned int getpbuffrecivesize();
    int setobuffrecivesize(unsigned int size);
    int setmbuffrecivesize(unsigned int size);
    int setpbuffrecivesize(unsigned int size);

    char * mbuff;
    char * pbuff;
    char * obuff;

    int getblocksize();
    int getcachestatus();

protected:
    void run();
    int findindex();
    int createcache();
    bool checkmpty();
    bool checkfull();

private:
    int headp;
    int  outputp;
    int  inputp;
    unsigned int blocksize;
    unsigned int maxcachsize;
    unsigned int obuffsize;
    unsigned int obuffrecivesize;
    unsigned int mbuffrecivesize;
    unsigned int pbuffrecivesize;
    int countbyte;
    int readflag;
    int writeflag;
    datablock_t * cache;
};

#endif // CACHEBUF_H
