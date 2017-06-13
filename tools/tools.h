#ifndef TOOLS_H
#define TOOLS_H

#include "globaflags.h"

class Tools
{
public:
    Tools();
    ~Tools();
    int32_t myexec(std::string cmd, std::vector<std::string> &resvec);
    bool str_campare(const std::string str1, const std::string str2);
};

#endif // TOOLS_H
