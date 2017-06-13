#ifndef GETTIME_H
#define GETTIME_H
#include <iostream>
#include <time.h>
#include "globaflags.h"

class gettime
{
public:
    gettime();
    ~gettime();

    void getmouthofyear(std::string &mouth);
    void getdayofmouth(std::string &day);
    void getweekday(std::string &weekday);
    std::string gettimeasstring();
    std::string getdateasstring();
    uint calculatesubtime(std::string timeend, std::string timestart );
    std::string  gettimetickasstring();
};

#endif // GETTIME_H
