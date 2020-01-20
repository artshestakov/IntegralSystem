#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISDateEdit.h"
//-----------------------------------------------------------------------------
class ISBirthdayEdit : public ISDateEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBirthdayEdit(QWidget *parent);
	virtual ~ISBirthdayEdit();

	void SetValue(const QVariant &value) override;

protected:
	void UpdateLabel(const QDate &Date);
	void DateEnableChanged(int State) override;

private:
	QLabel *Label;
};
//-----------------------------------------------------------------------------
