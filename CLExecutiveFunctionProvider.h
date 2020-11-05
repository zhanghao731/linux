#ifndef CLEXECUTIVEFUNCTIONPROVIDER_H
#define CLEXECUTIVEFUNCTIONPROVIDER_H


//执行体功能的接口类，线程业务逻辑的提供者
class CLExecutiveFunctionProvider{
public:
    CLExecutiveFunctionProvider(){}
    virtual ~CLExecutiveFunctionProvider(){}
public:
    virtual void RunExecutiveFunction()=0;
};

#endif // CLEXECUTIVEFUNCTIONPROVIDER_H
