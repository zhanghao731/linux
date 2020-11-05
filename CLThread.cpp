#include "CLThread.h"

CLThread::CLThread(CLExecutiveFunctionProvider* pExecutiveFunctionProvider):CLExecutive(pExecutiveFunctionProvider) {
}

CLThread::~CLThread() {
}

void* CLThread::StartFunctionOfThread(void *pThis){

    CLThread* pThreadThis=(CLThread *)pThis;
    std::cout<<std::endl<<"线程ID："<<pThreadThis->m_ThreadID<<std::endl;
    pThreadThis->m_pExecutiveFunctionProvider->RunExecutiveFunction();
    return 0;
}

void CLThread::Run(){
    int r=pthread_create(&m_ThreadID,0,StartFunctionOfThread,this);

    if(r!=0)
    {
        std::cout<<"pthread_create error"<<std::endl;
        return ;
    }
}

void CLThread::WaitForDeath(){
    int r=pthread_join(m_ThreadID,0);
    if(r!=0)
    {
        std::cout<<"In CLThread::WaitForDeath(),pthread_join error"<<std::endl;
        return ;
    }
}




