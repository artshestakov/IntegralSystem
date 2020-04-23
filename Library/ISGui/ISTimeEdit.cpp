#include "ISTimeEdit.h"
//-----------------------------------------------------------------------------
ISTimeEdit::ISTimeEdit(QWidget *parent) : ISDateTimeEdit(parent)
{
	SetVisibleDateEdit(false);
}
//-----------------------------------------------------------------------------
ISTimeEdit::~ISTimeEdit()
{

}
//-----------------------------------------------------------------------------
void ISTimeEdit::SetValue(const QVariant &value)
{
	ISDateTimeEdit::SetValue(QDateTime(QDate(), value.toTime()));
}
//-----------------------------------------------------------------------------
QVariant ISTimeEdit::GetValue() const
{
	return ISDateTimeEdit::GetValue().toTime();
}
//-----------------------------------------------------------------------------
