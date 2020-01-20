#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISProcessForm : public ISInterfaceForm
{
	Q_OBJECT

public:
    ISProcessForm(const QString &Text = QString(), QWidget *parent = 0);
	virtual ~ISProcessForm();

	void SetText(const QString &Text);

protected:
	void showEvent(QShowEvent *e);

private:
	QLabel *LabelText;
};
//-----------------------------------------------------------------------------
