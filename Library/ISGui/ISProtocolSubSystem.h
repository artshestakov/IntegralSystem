#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISProtocolSubSystem : public ISListBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISProtocolSubSystem(QWidget *parent = 0);
    virtual ~ISProtocolSubSystem();

protected:
    void DoubleClickedTable(const QModelIndex &ModelIndex);
};
//-----------------------------------------------------------------------------
