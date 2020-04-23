#pragma once
#ifndef _ISDOUBLEEDIT_H_INCLUDED
#define _ISDOUBLEEDIT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISLineEdit.h"
#include "ISEdits.h"
#include "ISValidators.h"
//-----------------------------------------------------------------------------
class ISDoubleEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDoubleEdit(QWidget *parent);
	virtual ~ISDoubleEdit();

	void SetValue(const QVariant &value) override;
	void SetRange(double Minimum, double Maximum);

private:
	ISDoubleValidator *DoubleValidator;
};
//-----------------------------------------------------------------------------
#endif
