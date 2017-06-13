#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG
//#undef DEBUG

#define DEBUGLOG
#define DEBUGLOGOFNETWORK

#ifdef  DEBUG
#define DEBUG1INFO(string); printf(string)
#define DEBUG2INFO(string,data); printf(string,data)
#define DEBUG3INFO(string,data,data2); printf(string,data,data2)
#else
#define DEBUG1INFO(string);
#define DEBUG2INFO(string,data);
#define DEBUG3INFO(string,data,data2);
#endif

#ifdef DEBUGLOG
#define LOG_D(string)
#else


#endif

//#undef DEBUGLOGOFNETWORK
#ifdef DEBUGLOGOFNETWORK
#define DEBUGLOGOFNETWORK1INFO(string); printf(string);
#define DEBUGLOGOFNETWORK2INFO(string,data); printf(string,data);
#define DEBUGLOGOFNETWORK3INFO(string,data,data2); printf(string,data,data2);
#else
#define DEBUGLOGOFNETWORK1INFO(string);
#define DEBUGLOGOFNETWORK2INFO(string,data);
#define DEBUGLOGOFNETWORK3INFO(string,data,data2);

#endif

#define LOG_STORAGE(string , logfile)   do {\
                                           \
                                        char buf[200]; \
                                        sprintf(buf,"echo %s >> %s",string.c_str() , logfile.c_str()); \
                                        system(buf); \
                                           \
                                        } while(0)


enum debugleavel  {
    debuginfo,
    debugnormal,
    debugurgency,
    debugerror
};


class debug
{
public:
    debug();
    ~debug();

};

#endif // DEBUG_H
