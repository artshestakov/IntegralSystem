#pragma once
#ifndef _ISASTERISKCALLSSUBSYSTEM_H_INCLUDED
#define _ISASTERISKCALLSSUBSYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISAsteriskCallsSubSystem : public ISListBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISAsteriskCallsSubSystem(QWidget *parent = 0);
    virtual ~ISAsteriskCallsSubSystem();

private:
    void PlayRecordCall();
};
//-----------------------------------------------------------------------------
#endif
