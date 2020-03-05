#include "CLDateTimeEdit.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
CLDateTimeEdit::CLDateTimeEdit(QWidget *parent) : QWidget(parent)
{
	setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
    setSizePolicy(QSizePolicy::Maximum, sizePolicy().verticalPolicy());

    QHBoxLayout *Layout = new QHBoxLayout();
    Layout->setContentsMargins(0, 0, 0, 0);
    setLayout(Layout);

    DateEdit = new CLDateEdit(this);
	connect(DateEdit, &CLDateEdit::DateChanged, this, &CLDateTimeEdit::DateTimeChanged);
    Layout->addWidget(DateEdit);

    TimeEdit = new CLTimeEdit(this);
	connect(TimeEdit, &CLTimeEdit::TimeChanged, this, &CLDateTimeEdit::DateTimeChanged);
    Layout->addWidget(TimeEdit);
}
//-----------------------------------------------------------------------------
CLDateTimeEdit::~CLDateTimeEdit()
{

}
//-----------------------------------------------------------------------------
QDateTime CLDateTimeEdit::GetDateTime() const
{
    return QDateTime(DateEdit->GetDate(), TimeEdit->GetTime());
}
//-----------------------------------------------------------------------------
void CLDateTimeEdit::SetDateTime(const QDateTime &DateTime)
{
    DateEdit->SetDate(DateTime.date());
    TimeEdit->SetTime(DateTime.time());
}
//-----------------------------------------------------------------------------
void CLDateTimeEdit::SetDate(const QDate &Date)
{
    DateEdit->SetDate(Date);
}
//-----------------------------------------------------------------------------
void CLDateTimeEdit::SetTime(const QTime &Time)
{
    TimeEdit->SetTime(Time);
}
//-----------------------------------------------------------------------------
void CLDateTimeEdit::Clear()
{
    DateEdit->Clear();
    TimeEdit->Clear();
}
//-----------------------------------------------------------------------------
void CLDateTimeEdit::SetVisibleDateEdit(bool Visible)
{
    DateEdit->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void CLDateTimeEdit::SetVisibleTimeEdit(bool Visible)
{
    TimeEdit->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void CLDateTimeEdit::SetRangeDate(const QDate &Minimum, const QDate &Maximum)
{
	DateEdit->SetRange(Minimum, Maximum);
}
//-----------------------------------------------------------------------------
