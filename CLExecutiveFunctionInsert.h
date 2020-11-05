#ifndef CLEXECUTIVEFUNCTIONINSERT_H
#define CLEXECUTIVEFUNCTIONINSERT_H
#include "CLExecutiveFunctionProvider.h"
#include "table.h"

class CLExecutiveFunctionInsert: public CLExecutiveFunctionProvider {
public:
    CLExecutiveFunctionInsert() {}

    virtual ~CLExecutiveFunctionInsert() {}

    virtual void RunExecutiveFunction() {
        table* m_table = table::GetTable();
        m_table->InsertRecord();
    }
};


#endif // CLEXECUTIVEFUNCTIONINSERT_H
