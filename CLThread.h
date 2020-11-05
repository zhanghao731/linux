#ifndef CLTHREAD_H
#define CLTHREAD_H

#include<pthread.h>
#include<iostream>
#include "CLExecutive.h"

class CLThread : public CLExecutive
{
public:
    explicit CLThread(CLExecutiveFunctionProvider* pExecutiveFunctionProvider);
    virtual ~CLThread();
    virtual void Run();
    virtual void WaitForDeath();

private:
    static void* StartFunctionOfThread(void* pThis);

private:
    pthread_t m_ThreadID;
};


#endif // CLTHREAD_H
