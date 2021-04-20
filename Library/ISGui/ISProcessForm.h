#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISProcessForm : public ISInterfaceForm
{
    Q_OBJECT

public:
    ISProcessForm(const QString &LabelText = QString(), QWidget *parent = 0);
    virtual ~ISProcessForm();

    void SetText(const QString &LabelText);

protected:
    void showEvent(QShowEvent *ShowEvent);

private:
    QLabel *Label;
};
//-----------------------------------------------------------------------------
