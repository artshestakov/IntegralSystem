#include "ISDoubleEdit.h"
#include "ISDefinesCore.h"
#include "ISSettingsDatabase.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISDoubleEdit::ISDoubleEdit(QWidget *parent) : ISLineEdit(parent)
{
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
	Ok ? ISLineEdit::SetValue(QString::fromStdString(ISAlgorithm::PrepareDouble(Double, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA)))) :
		ISLineEdit::SetValue(QString());
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::TextChanged(const QString &Text)
{
	QString String = Text;
	ISAlgorithm::PrepareStringDouble(String, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA));
	if (String != Text)
	{
		ISLineEdit::TextChangedDisconnect();
		ISLineEdit::SetValue(String);
		ISLineEdit::TextChangedConnect();
	}
	ISLineEdit::TextChanged(String);
}
//-----------------------------------------------------------------------------
