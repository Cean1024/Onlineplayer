#include "gettime.h"

gettime::gettime()
{

}

gettime::~gettime()
{

}


void gettime::getmouthofyear(std::string &mouth)
 {
     time_t local_time;
     struct tm *timeval;

     char buf[50];

     time(&local_time);
     memset(buf,0,50);
     timeval = localtime(&local_time);

     strftime(buf,50,"%m",timeval);
     mouth = buf;

}

void gettime::getdayofmouth(std::string &day)
 {
     time_t local_time;
     struct tm *timeval;

     char buf[50];

     time(&local_time);
     memset(buf,0,50);
     timeval = localtime(&local_time);

     strftime(buf,50,"%d",timeval);
     day = buf;

}

void gettime::getweekday(std::string &weekday)
 {
     time_t local_time;
     struct tm *timeval;

     char buf[50];

     time(&local_time);
     memset(buf,0,50);
     timeval = localtime(&local_time);

     strftime(buf,50,"%w",timeval);
     weekday = buf;

}

std::string  gettime::gettimetickasstring()
{
    time_t local_time;
    //struct tm *timeval;
    char buf[50];

    time(&local_time);
    memset(buf,0,50);
    sprintf(buf,"%lu",local_time);
    std::string times = buf;
    return times;
}

std::string gettime::gettimeasstring()
{
    time_t local_time;
    struct tm *timeval;
    char buf[50];

    time(&local_time);
    memset(buf,0,50);
    timeval = localtime(&local_time);

    strftime(buf,50,"%T",timeval);
    //std::string times = asctime(timeval);
    std::string times = buf;
    return times;
}
std::string gettime::getdateasstring()
{
    time_t local_time;
    struct tm *timeval;
    char buf[50];

    time(&local_time);
    memset(buf,0,50);
    timeval = localtime(&local_time);

    strftime(buf,50,"%F",timeval);
    //std::string times = asctime(timeval);
    std::string times = buf;
    return times;
}

/*
 *
 * time format : 00:00:00/00-00-00/00 00 00;
 *
 */
uint gettime::calculatesubtime(std::string timeend ,std::string timestart )
{
    uint timeticka;
    uint timetickb;

    //DEBUG2INFO( "datestr : %s\n",timestart.c_str() );
    timeticka = (10 * (timeend[0] - '0') + (timeend[1] - '0')) * 3600 +
            (10 * (timeend[3] - '0') + (timeend[4] - '0')) * 60 +
            (10 * (timeend[6] - '0') + (timeend[7] - '0'));

    timetickb = (10 * (timestart[0] - '0') + (timestart[1] - '0')) * 3600 +
            (10 * (timestart[3] - '0') + (timestart[4] - '0')) * 60 +
            (10 * (timestart[6] - '0') + (timestart[7] - '0'));

    timeticka = timeticka - timetickb;
    return timeticka;

}

