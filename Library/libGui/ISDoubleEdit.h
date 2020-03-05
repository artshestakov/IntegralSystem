#pragma once
//-----------------------------------------------------------------------------
#include "ISLineEdit.h"
#include "ISEdits.h"
//-----------------------------------------------------------------------------
class ISDoubleEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDoubleEdit(QWidget *parent);
	virtual ~ISDoubleEdit();

	QVariant GetValue() const override;
	void SetRange(double Minimum, double Maximum);

private:
	QDoubleValidator *DoubleValidator;
};
//-----------------------------------------------------------------------------
