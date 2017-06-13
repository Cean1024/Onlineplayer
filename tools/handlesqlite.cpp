#include "handlesqlite.h"




HandleSqlite::HandleSqlite()
{
    dbid = NULL;

}

HandleSqlite::~HandleSqlite()
{

}
#if 0
#define playlistsectionsize 12
#define playlistitemsize 8

static std::string  playlist_section[ playlistsectionsize ] =
{ "id"       ,"name"   ,"level"    ,"datestr" ,
  "dateparem","itemcnt","datetype" ,"looptype",
  "loopcount","volume" ,"begintick","endtick" };


static std::string playlist_item[ playlistitemsize ] =
{ "id"     ,"section"  ,"format"   ,"fileid"   ,
  "title"  ,"artist"   ,"album"    ,"musictick"};
#endif


Json::Value * datas;
Json::FastWriter writer;
int show_sql_result(void *arg, int n_column,\
                    char **column_value, char **column_name)
{
    if( arg != NULL ) {

        Json::Value data;
        char tmp[20];
        datas = (Json::Value * )arg;
        int num;
        num = datas->size();
        //num++;
        sprintf(tmp,"%d",num);

        for(int i =0;i < n_column ;i++) {
           data[ column_name[i] ] = column_value[i];
        }

        (*datas)[tmp] = data;


    } else {
        for(int i =0;i < n_column ;i++){

            DEBUG3INFO("< %s | %s >\n",column_name[i] , column_value[i]);
        }
    }
    return SUCCESS;
}
int exec_sql(std::string &sql_string, sqlite3 *db ,void *param)
{
    char *errmsg = NULL;

    if( SQLITE_OK != sqlite3_exec (
                db,		/* handler to the db connection */
                sql_string.c_str(), 	/* SQL statements */
                show_sql_result,/* callback */
                param, 		/* param to the callback */
                &errmsg)) {	/* error message holder, NULL means you don't want it */
        fprintf(stderr, "sqlite3_exec err: %s.\n", errmsg);
        sqlite3_free(errmsg);
        return FAILED;
    }
    return SUCCESS;
}

int HandleSqlite::opendatabase(std::string dbfile)
{
    int ret = sqlite3_open(dbfile.c_str(), &dbid);
    if (ret != SQLITE_OK) {
        if (NULL != dbid) {
            fprintf(stderr, "sqlite3_open %s : %s.\n",
                    dbfile.c_str(), sqlite3_errmsg(dbid));
        } else {
            printf("error : failed to allocate memory for sqlite3!\n");
        }
        return FAILED;
    }
    return SUCCESS;

}

int HandleSqlite::runsqlitecmd(std::string &cmd)
{

    if( exec_sql(cmd , dbid , NULL) == FAILED )
        return FAILED;

    return SUCCESS;

}

int HandleSqlite::closedatabase()
{
    if(dbid == NULL )
        return FAILED;

    int ret = sqlite3_close(dbid);
    if (ret != SQLITE_OK) {

        printf("error : failed to close sqlite3!\n");

        return FAILED;
    }
    dbid = NULL;
    return SUCCESS;
}

int HandleSqlite::getdatafromedb(std::string table, std::string name ,
                                 std::string value, Json::Value &datas)
{
    std::string cmd;

    if(dbid == NULL )
        return FAILED;

    cmd = "select * from " + table + " where " + name+"=\""+value + "\"";

    if( exec_sql(cmd , dbid , &datas) == FAILED )
        return FAILED;

    return SUCCESS;
}

int  HandleSqlite::gettabledata(std::string table  , Json::Value &datas)
{
    std::string cmd;

    if(dbid == NULL )
        return FAILED;

    cmd = "select * from " + table;

    if( exec_sql(cmd , dbid , &datas)  == FAILED ){
        printf("gettabledata failed!\n");
        return FAILED;
    }

    return SUCCESS;
}
