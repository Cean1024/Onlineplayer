#include "managerom.h"

ManageRom::ManageRom()
{

}

ManageRom::~ManageRom()
{

}



r_status ManageRom::cleanrom(std::string &workdir)
{
    this->workfolder = workdir;

    this->start();

    return SUCCESS;

}
void ManageRom::run()
{
    r_status ret;
    std::string fileid,filenameandpath ,cmd;
    Json::Value resoult;
    Json::Value onedate;
    int size;
    char buf[10];

    sleep(1);
    ret = sqlitetool.opendatabase(LOCALDBFILENAME);
    if(ret == FAILED)
        return ;
    sqlitetool.getdatafromedb("playlist_item","stat","0",resoult);
    sqlitetool.closedatabase();
    size = resoult.size();
    //printf(" ManageRom::run size :%d\n",size);
    for(int i =0;i < size ;i++) {
        sprintf(buf,"%d",i);
        onedate = resoult[buf];
        fileid = onedate["fileid"].asString();
        filenameandpath = workfolder + MUSICESTORAGEFOLDER + "/" + fileid;
        cmd = "rm ";
        cmd +=filenameandpath;
        std::cout << cmd << std::endl;
        system( cmd.c_str() ) ;
    }
}
