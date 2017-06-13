#ifndef UPDATE_H
#define UPDATE_H

#include "network/netlink.h"
#include "thread.h"
#include "tools/tools.h"
#include <dirent.h>
#include <sys/types.h>

#define DFWAITECOUNT 20


enum hotplugstat{
    ADDDEVICE = 0,
    REMOVEDEVICE

};

class Update:public Thread
{
public:
    Update();
    ~Update();

    static Update * getInstance()
    {
        static Update updates;
        return &updates;
    }

    r_status init();

protected:
    void run();
    r_status handlerecivedate(Netlink &dataNL, string &recv, hotplugstat &status);
    void HandleAddRemove(std::string &recv , hotplugstat &status);
    void HandleAddDevice(std::string &recv);

private:
    Netlink dataNL;

};

#endif // UPDATE_H
