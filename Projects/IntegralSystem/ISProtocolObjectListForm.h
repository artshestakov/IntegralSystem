#pragma once
#ifndef _ISPROTOCOLOBJECTLISTFORM_H_INCLUDED
#define _ISPROTOCOLOBJECTLISTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISSubSystem.h"
//-----------------------------------------------------------------------------
class ISProtocolObjectListForm : public ISProtocolSubSystem
{
    Q_OBJECT

public:
    Q_INVOKABLE ISProtocolObjectListForm(QWidget *parent = 0);
    virtual ~ISProtocolObjectListForm();

protected:
    virtual bool Update() override;
    void LoadData() override;
};
//-----------------------------------------------------------------------------
#endif
