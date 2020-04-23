#include "ISDoubleEdit.h"
#include "ISDefinesCore.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
ISDoubleEdit::ISDoubleEdit(QWidget *parent) : ISLineEdit(parent)
{
	DoubleValidator = new ISDoubleValidator(SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA), this);
	SetValidator(DoubleValidator);

	SetSizePolicyHorizontal(QSizePolicy::Maximum);
}
//-----------------------------------------------------------------------------
ISDoubleEdit::~ISDoubleEdit()
{

}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetValue(const QVariant &value)
{
	bool Ok = true;
	double Double = value.toDouble(&Ok);
	if (Ok)
	{
		std::stringstream StringStream;
		StringStream << std::setprecision(SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA) + 1) << Double;
		std::string String = StringStream.str();
		ISLineEdit::SetValue(String.c_str());
	}
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetRange(double Minimum, double Maximum)
{
	DoubleValidator->setBottom(Minimum);
	DoubleValidator->setTop(Maximum);
}
//-----------------------------------------------------------------------------
