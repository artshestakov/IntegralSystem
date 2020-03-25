#include "ISDateTimeEdit.h"
#include "ISConstants.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISDateTimeEdit::ISDateTimeEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
	CreateButtonClear();

    QHBoxLayout *Layout = new QHBoxLayout();
    Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *Widget = new QWidget(this);
	Widget->setLayout(Layout);
	AddWidgetEdit(Widget, this);

	DateEdit = new ISQDateEdit(this);
	connect(DateEdit, &ISQDateEdit::DateChanged, this, &ISDateTimeEdit::ValueChanged);
	Layout->addWidget(DateEdit);

    TimeEdit = new ISQTimeEdit(this);
	connect(TimeEdit, &ISQTimeEdit::TimeChanged, this, &ISDateTimeEdit::ValueChanged);
    Layout->addWidget(TimeEdit);
}
//-----------------------------------------------------------------------------
ISDateTimeEdit::~ISDateTimeEdit()
{

}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetValue(const QVariant &value)
{
	QDateTime DateTime = value.toDateTime();
	if (!DateTime.date().isNull())
	{
		DateEdit->SetDate(DateTime.date());
	}
	if (!DateTime.time().isNull())
	{
		TimeEdit->SetTime(DateTime.time());
	}
}
//-----------------------------------------------------------------------------
QVariant ISDateTimeEdit::GetValue() const
{
	return QDateTime(DateEdit->GetDate(), TimeEdit->GetTime());
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::Clear()
{
	DateEdit->Clear();
	TimeEdit->Clear();
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetVisibleClear(bool Visible)
{
	ISFieldEditBase::SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetDate(const QDate &Date)
{
    DateEdit->SetDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetTime(const QTime &Time)
{
    TimeEdit->SetTime(Time);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetRangeDate(const QDate &Minimum, const QDate &Maximum)
{
	DateEdit->SetRange(Minimum, Maximum);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetMinimumDate(const QDate &Date)
{
	DateEdit->SetMinimumDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetMaximumDate(const QDate &Date)
{
	DateEdit->SetMaximumDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetVisibleDateEdit(bool Visible)
{
    DateEdit->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetVisibleTimeEdit(bool Visible)
{
    TimeEdit->setVisible(Visible);
}
//-----------------------------------------------------------------------------
