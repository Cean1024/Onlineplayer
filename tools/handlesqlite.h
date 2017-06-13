#ifndef HANDLESQLITE_H
#define HANDLESQLITE_H
#include "sqlite3.h"
#include "globaflags.h"

class HandleSqlite
{
public:
    HandleSqlite();
    ~HandleSqlite();
    int opendatabase(std::string dbfile);
    int getdatafromedb(std::string table, std::string name,
                       std::string value, Json::Value &datas);
    int gettabledata(std::string table, Json::Value &datas);
    int closedatabase();
    int runsqlitecmd(std::string &cmd);
protected:


private:
    sqlite3 * dbid;

};

#endif // HANDLESQLITE_H
