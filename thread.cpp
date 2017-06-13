#include "thread.h"

Thread::Thread()
{
    tid = 0;
    THREAD_STATUS_NEW = threadstatusnew;
    //线程的状态－正在运行
    THREAD_STATUS_RUNNING = threadstatusruning;
    //线程的状态－运行结束
    THREAD_STATUS_EXIT = threadstatusexit;

    threadStatus = THREAD_STATUS_EXIT;
    data_thread = -1;
}


Thread::~Thread()
{

}

void* Thread::run0(void* pVoid)
{
    Thread* p = (Thread*) pVoid;
    p->run1();
    return p;
}

void* Thread::run1()
{

    threadStatus = THREAD_STATUS_RUNNING;
    tid = pthread_self();
    pthread_detach(tid);
    //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);           //允许退出线程
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,   NULL);    //设置立即取消
    run();
    threadStatus = THREAD_STATUS_EXIT;
    tid = 0;
    pthread_exit(NULL);
}

bool Thread::start()
{
    ret = pthread_create(&tid, NULL, run0, this);

    return  ret == 0;
}

pthread_t Thread::getThreadID()
{
    return tid;
}

int Thread::getState()
{
    return threadStatus;
}

void Thread::setdata(int dat)
{
    data_thread = dat;
}

int Thread::getdata()
{
    return data_thread;
}

void Thread::join()
{
    if (tid > 0)
    {
        pthread_join(tid, NULL);
        threadStatus = THREAD_STATUS_EXIT;
        tid = 0;
    }
}

void Thread::join(unsigned long millisTime)
{

    if (tid == 0)
    {
        return;
    }
    if (millisTime == 0)
    {
        join();
    }else
    {
        unsigned long k = 0;
        while (threadStatus != THREAD_STATUS_EXIT && k <= millisTime)
        {
            usleep(100);
            k++;
        }
    }
}

int Thread::distroy()
{

    if (tid > 0)
    {
        pthread_cancel(tid);
        //pthread_join(tid, NULL);
        threadStatus = THREAD_STATUS_EXIT;
        tid = 0;
        return PSUCCESS;
    }
    return PFAILED;
}
