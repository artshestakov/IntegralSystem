#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISCalendarObjectForm : public ISObjectFormBase
{
    Q_OBJECT

public:
    Q_INVOKABLE ISCalendarObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
    virtual ~ISCalendarObjectForm();

    bool Save() override;

protected:
    void AfterShowEvent() override;
    void EscapeClicked() override;
};
//-----------------------------------------------------------------------------
