#include "ISIntegerEdit.h"
#include "ISDefinesCore.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISIntegerEdit::ISIntegerEdit(QWidget *parent)
	: ISLineEdit(parent),
	IntValidator(new QIntValidator(this))
{
	SetValidator(IntValidator);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
}
//-----------------------------------------------------------------------------
ISIntegerEdit::~ISIntegerEdit()
{

}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetValue(const QVariant &value)
{
	ISLineEdit::SetValue(value.toString().isEmpty() ? QVariant() : value.toInt());
}
//-----------------------------------------------------------------------------
QVariant ISIntegerEdit::GetValue() const
{
	QString Value = ISLineEdit::GetValue().toString();
	return Value.isEmpty() ? QVariant() : QVariant(Value.toInt());
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetMinimum(int Minimum)
{
	SetRange(Minimum, INT_MAX);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetMaximum(int Maximum)
{
	SetRange(INT_MIN, Maximum);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetRange(int Minimum, int Maximum)
{
	if (IntValidator)
	{
		delete IntValidator;
	}

	IntValidator = new QIntValidator(Minimum, Maximum, this);
	SetValidator(IntValidator);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::ResetRange()
{
	SetRange(INT_MIN, INT_MAX);
}
//-----------------------------------------------------------------------------
