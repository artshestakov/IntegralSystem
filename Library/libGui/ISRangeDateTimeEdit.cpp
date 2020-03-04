#include "ISRangeDateTimeEdit.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISRangeDateTimeEdit::ISRangeDateTimeEdit(QWidget *parent, const QString &FieldName) : ISRangeEditBase(parent, FieldName)
{
	BeginDateTimeEdit = new ISDateTimeEdit(this);
	BeginDateTimeEdit->SetChecked(true);
	BeginDateTimeEdit->SetVisibleCheck(false);

	EndDateTimeEdit = new ISDateTimeEdit(this);
	EndDateTimeEdit->SetChecked(true);
	EndDateTimeEdit->SetVisibleCheck(false);

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
	ISRangeStruct Range;
	Range.BeginValue = BeginDateTimeEdit->GetValue();
	Range.EndValue = EndDateTimeEdit->GetValue();
	return Range;
}
//-----------------------------------------------------------------------------
