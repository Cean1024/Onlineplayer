#ifndef DOCACHE_H
#define DOCACHE_H

#include "globaflags.h"
#define  CACHEFOLD "download"
class docache
{
public:
    docache();
    ~docache();

    int cacheinit();
    int inputdata();
    int outputdata();
    int closecache();
    int opencache();
    int searchcache();

};

#endif // DOCACHE_H
