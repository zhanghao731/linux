#ifndef CLEXECUTIVEFUNCTIONSEARCH_H
#define CLEXECUTIVEFUNCTIONSEARCH_H

#include "table.h"
#include "CLExecutiveFunctionProvider.h"

class CLExecutiveFunctionSearch:public CLExecutiveFunctionProvider{
public:

    CLExecutiveFunctionSearch(int left,int right,int col){
        m_left=left;
        m_right=right;
        m_col=col;
    }
    virtual ~CLExecutiveFunctionSearch(){}
    virtual void RunExecutiveFunction(){
        table* m_table=table::GetTable();
        m_table->SearchRecord(m_left,m_right,m_col);
    }

private:
    //查询信息
    int m_left;
    int m_right;
    int m_col;
};



#endif // CLEXECUTIVEFUNCTIONSEARCH_H
