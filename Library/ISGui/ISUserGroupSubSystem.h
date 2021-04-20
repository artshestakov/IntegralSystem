#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISUserGroupSubSystem : public ISListBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISUserGroupSubSystem(QWidget *parent = 0);
    virtual ~ISUserGroupSubSystem();

private:
    void SelectedGroup();
};
//-----------------------------------------------------------------------------
