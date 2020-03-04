#include "ISRangeDateEdit.h"
#include "ISLocalization.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
ISRangeDateEdit::ISRangeDateEdit(QWidget *parent, const QString &FieldName) : ISRangeEditBase(parent, FieldName)
{
	ISComboEdit *ComboEdit = new ISComboEdit(this);
	ComboEdit->SetEditable(false);
	ComboEdit->AddItem(LANG("Range.Date.Select"), ISNamespace::RangeDatePeriod::RDP_Unknown);
	ComboEdit->AddItem(LANG("Range.Date.Today"), ISNamespace::RangeDatePeriod::RDP_Today);
	ComboEdit->AddItem(LANG("Range.Date.Yesterday"), ISNamespace::RangeDatePeriod::RDP_Yesterday);
	ComboEdit->AddItem(LANG("Range.Date.CurrentMonth"), ISNamespace::RangeDatePeriod::RDP_CurrentMonth);
	ComboEdit->AddItem(LANG("Range.Date.PreviousMonth"), ISNamespace::RangeDatePeriod::RDP_LastMonth);
	ComboEdit->AddItem(LANG("Range.Date.CurrentYear"), ISNamespace::RangeDatePeriod::RDP_CurrentYear);
	ComboEdit->AddItem(LANG("Range.Date.PreviousYear"), ISNamespace::RangeDatePeriod::RDP_LastYear);
	ComboEdit->AddItem(LANG("Range.Date.Other"), ISNamespace::RangeDatePeriod::RDP_Other);
	connect(ComboEdit, &ISComboEdit::ValueChange, this, &ISRangeDateEdit::PeriodChanged);
	AddWidgetToLeft(ComboEdit);

	BeginDateEdit = new ISDateEdit(this);
	BeginDateEdit->SetChecked(true);
	BeginDateEdit->SetVisibleCheck(false);

	EndDateEdit = new ISDateEdit(this);
	EndDateEdit->SetChecked(true);
	EndDateEdit->SetVisibleCheck(false);

	AddFields(LANG("FieldRange.DateTime.Begin"), BeginDateEdit, LANG("FieldRange.DateTime.End"), EndDateEdit);
}
//-----------------------------------------------------------------------------
ISRangeDateEdit::~ISRangeDateEdit()
{

}
//-----------------------------------------------------------------------------
void ISRangeDateEdit::SetValue(const ISRangeStruct &range)
{
	BeginDateEdit->SetValue(range.BeginValue.toDate());
	EndDateEdit->SetValue(range.EndValue.toDate());
}
//-----------------------------------------------------------------------------
ISRangeStruct ISRangeDateEdit::GetValue() const
{
	return ISRangeStruct(BeginDateEdit->GetValue(), EndDateEdit->GetValue());
}
//-----------------------------------------------------------------------------
void ISRangeDateEdit::PeriodChanged(const QVariant &value)
{
	ISNamespace::RangeDatePeriod SelectedPeriod = qvariant_cast<ISNamespace::RangeDatePeriod>(value);
	ISRangeStruct RangeStruct;
	if (SelectedPeriod == ISNamespace::RDP_Today)
	{
		RangeStruct.BeginValue = QDate::currentDate();
		RangeStruct.EndValue = QDate::currentDate();
	}
	else if (SelectedPeriod == ISNamespace::RDP_Yesterday)
	{
		RangeStruct.BeginValue = QDate::currentDate().addDays(-1);
		RangeStruct.EndValue = QDate::currentDate().addDays(-1);
	}
	else if (SelectedPeriod == ISNamespace::RDP_CurrentMonth)
	{
		RangeStruct.BeginValue = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
		RangeStruct.EndValue = QDate(QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().daysInMonth());
	}
	else if (SelectedPeriod == ISNamespace::RDP_LastMonth)
	{
		QDate Date = QDate::currentDate().addMonths(-1);
		RangeStruct.BeginValue = QDate(Date.year(), Date.month(), 1);
		RangeStruct.EndValue = QDate(Date.year(), Date.month(), Date.daysInMonth());
	}
	else if (SelectedPeriod == ISNamespace::RDP_CurrentYear)
	{
		RangeStruct.BeginValue = QDate(QDate::currentDate().year(), 1, 1);
		RangeStruct.EndValue = QDate(QDate::currentDate().year(), 12, 31);
	}
	else if (SelectedPeriod == ISNamespace::RDP_LastYear)
	{
		QDate Date = QDate::currentDate().addYears(-1);
		RangeStruct.BeginValue = QDate(Date.year(), 1, 1);
		RangeStruct.EndValue = QDate(Date.year(), 12, 31);
	}

	SetValue(RangeStruct);
	BeginDateEdit->SetFocus();
}
//-----------------------------------------------------------------------------
