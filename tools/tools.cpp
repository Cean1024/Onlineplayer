#include "tools.h"

Tools::Tools()
{

}

Tools::~Tools()
{

}
int32_t Tools::myexec (std::string cmd, std::vector<std::string> &resvec)
{
    resvec.clear();
    FILE *pp = popen(cmd.c_str(), "r"); //建立管道
    if (!pp) {
        return -1;
    }
    char tmp[ 1024 ]; //设置一个合适的长度，以存储每一行输出
    while (fgets(tmp, sizeof(tmp), pp) != NULL) {
        if (tmp[strlen(tmp) - 1] == '\n') {
            tmp[strlen(tmp) - 1] = '\0'; //去除换行符
        }
        resvec.push_back(tmp);
    }
    pclose(pp); //关闭管道
    return resvec.size();
}

bool Tools::str_campare(const std::string str1, const std::string str2)
{
    int i = 0;
    int len1 = str1.length();
    int len2 = str2.length();
    if (len1 != len2)
        return false;
    while(i != len1)
    {
        if (!(str1[i] == str2[i] || ('a' -'A') == (str1[i] - str2[i]) || ('A' -'a') == (str1[i] - str2[i]) ))
            return false;
        else
            i++;
    }
    if (i == len1)
        return true;
}

