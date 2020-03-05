#include "ISRangeDateTimeEdit.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISRangeDateTimeEdit::ISRangeDateTimeEdit(QWidget *parent, const QString &FieldName) : ISRangeEditBase(parent, FieldName)
{
	BeginDateTimeEdit = new ISDateTimeEdit(this), EndDateTimeEdit = new ISDateTimeEdit(this);
	AddFields(LANG("FieldRange.DateTime.Begin"), BeginDateTimeEdit, LANG("FieldRange.DateTime.End"), EndDateTimeEdit);
}
//-----------------------------------------------------------------------------
ISRangeDateTimeEdit::~ISRangeDateTimeEdit()
{

}
//-----------------------------------------------------------------------------
void ISRangeDateTimeEdit::SetValue(const ISRangeStruct &range)
{
	BeginDateTimeEdit->SetValue(range.BeginValue.toDateTime());
	EndDateTimeEdit->SetValue(range.EndValue.toDateTime());
}
//-----------------------------------------------------------------------------
ISRangeStruct ISRangeDateTimeEdit::GetValue() const
{
	return ISRangeStruct{ BeginDateTimeEdit->GetValue(),EndDateTimeEdit->GetValue() };
}
//-----------------------------------------------------------------------------
