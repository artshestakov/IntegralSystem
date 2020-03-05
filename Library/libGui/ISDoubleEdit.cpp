#include "ISDoubleEdit.h"
#include "ISDefinesCore.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISDoubleEdit::ISDoubleEdit(QWidget *parent)
	: ISLineEdit(parent),
	DoubleValidator(new QDoubleValidator(this))
{
	DoubleValidator->setLocale(QLocale(QLocale::English));
	SetValidator(DoubleValidator);
	SetFixedWidth(150);
	AddMainLayoutStretch();
}
//-----------------------------------------------------------------------------
ISDoubleEdit::~ISDoubleEdit()
{

}
//-----------------------------------------------------------------------------
QVariant ISDoubleEdit::GetValue() const
{
	QVariant Value = ISLineEdit::GetValue();
	if (Value.isValid())
	{
		QString StringValue = Value.toString();
		if (StringValue.contains(SYMBOL_COMMA))
		{
			StringValue.replace(SYMBOL_COMMA, SYMBOL_POINT);
			Value.setValue<QString>(StringValue);
		}
	}
	return Value;
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetRange(double Minimum, double Maximum)
{
	if (DoubleValidator)
	{
		delete DoubleValidator;
	}
	
	DoubleValidator = new QDoubleValidator(Minimum, Maximum, DOUBLE_DECIMALS, this);
	SetValidator(DoubleValidator);
}
//-----------------------------------------------------------------------------
