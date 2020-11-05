#ifndef CLEXECUTIVE_H
#define CLEXECUTIVE_H

#include "CLExecutiveFunctionProvider.h"

//执行体的接口类
class CLExecutive{
public:
    explicit CLExecutive(CLExecutiveFunctionProvider* pExecutiveFunctionProvider){
        m_pExecutiveFunctionProvider=pExecutiveFunctionProvider;
    }
    virtual ~CLExecutive(){}
    virtual void Run()=0;
    virtual void WaitForDeath()=0;

protected:
    CLExecutiveFunctionProvider* m_pExecutiveFunctionProvider;

};


#endif // CLEXECUTIVE_H
