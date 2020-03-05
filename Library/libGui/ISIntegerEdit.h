#pragma once
//-----------------------------------------------------------------------------
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISIntegerEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISIntegerEdit(QWidget *parent);
	virtual ~ISIntegerEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;

	void SetMinimum(int Minimum);
	void SetMaximum(int Maximum);
	void SetRange(int Minimum, int Maximum);
	void ResetRange();

private:
	QIntValidator *IntValidator;
};
//-----------------------------------------------------------------------------
