#include "ISDoubleEdit.h"
#include "ISDefinesCore.h"
#include "ISSettingsDatabase.h"
#include "ISAlgorithm.h"
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
		ISLineEdit::SetValue(ISAlgorithm::PrepareDouble<double>(Double, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA)));
	}
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetRange(double Minimum, double Maximum)
{
	DoubleValidator->setBottom(Minimum);
	DoubleValidator->setTop(Maximum);
}
//-----------------------------------------------------------------------------
