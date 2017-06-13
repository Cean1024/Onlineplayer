#ifndef MANAGEROM_H
#define MANAGEROM_H
#include "globaflags.h"
#include "tools/handlesqlite.h"

class ManageRom:public Thread
{
public:


    ManageRom();
    ~ManageRom();

    r_status cleanrom(std::string &workdir);

protected:
    void run();

private:
    HandleSqlite sqlitetool;
    std::string workfolder;


};

#endif // MANAGEROM_H
