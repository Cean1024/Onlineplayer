#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <unistd.h>
#include "globaflags.h"

class Thread
{
private:
    int ret;
    //当前线程的线程ID
    pthread_t tid;
    //线程的状态
    int threadStatus;
    //获取执行方法的指针
    static void* run0(void* pVoid);
    //内部执行方法
    void* run1();

    //线程的状态－新建
    int THREAD_STATUS_NEW;
    //线程的状态－正在运行
    int THREAD_STATUS_RUNNING;
    //线程的状态－运行结束
    int THREAD_STATUS_EXIT;
    int data_thread;

public:
    Thread();
    ~Thread();


    //线程的运行实体
    virtual void run() = 0;
    //开始执行线程
    bool start();
    //获取线程ID
    pthread_t getThreadID();
    //获取线程状态
    int getState();
    //等待线程直至退出
    void join();
    //等待线程退出或者超时
    void join(unsigned long millisTime);
    int distroy();

    int getdata();
    void setdata(int dat);
};

#endif // THREAD_H
