#include "ISDateEdit.h"
//-----------------------------------------------------------------------------
ISDateEdit::ISDateEdit(QWidget *parent) : ISDateTimeEdit(parent)
{
	SetVisibleTimeEdit(false);
}
//-----------------------------------------------------------------------------
ISDateEdit::~ISDateEdit()
{

}
//-----------------------------------------------------------------------------
void ISDateEdit::SetValue(const QVariant &value)
{
	ISDateTimeEdit::SetValue(QDateTime(value.toDate(), QTime()));
}
//-----------------------------------------------------------------------------
QVariant ISDateEdit::GetValue() const
{
	return ISDateTimeEdit::GetValue().toDate();
}
//-----------------------------------------------------------------------------
void ISDateEdit::SetMinimumDate(const QDate &Date)
{
	ISDateTimeEdit::SetMinimumDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateEdit::SetMaximumDate(const QDate &Date)
{
	ISDateTimeEdit::SetMaximumDate(Date);
}
//-----------------------------------------------------------------------------
