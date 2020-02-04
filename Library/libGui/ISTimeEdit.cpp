#include "ISTimeEdit.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISTimeEdit::ISTimeEdit(QWidget *parent) : ISDateTimeEdit(parent)
{
	SetDisplayFormat(TIME_FORMAT_V1);
	SetVisibleCalendar(false);
	SetSelectSection(ISQDateTimeEdit::HourSection);
	SetReadOnly(false);

	ButtonTime = new ISServiceButton(this);
	ButtonTime->setEnabled(false);
	ButtonTime->setToolTip(LANG("ClickFromSelectDoneTime"));
	ButtonTime->setIcon(BUFFER_ICONS("Time"));
	ButtonTime->setMenu(new QMenu(ButtonTime));
	ButtonTime->setFocusPolicy(Qt::NoFocus);
	connect(ButtonTime, &ISServiceButton::clicked, this, &ISTimeEdit::SelectTime);
	AddWidgetToRight(ButtonTime);

	ButtonTime->menu()->addAction("7:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("8:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("9:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("10:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("11:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("12:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("13:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("14:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("15:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("16:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("17:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("18:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("19:00", this, &ISTimeEdit::SelectTime);
	ButtonTime->menu()->addAction("20:00", this, &ISTimeEdit::SelectTime);
}
//-----------------------------------------------------------------------------
ISTimeEdit::~ISTimeEdit()
{

}
//-----------------------------------------------------------------------------
void ISTimeEdit::SetValue(const QVariant &value)
{
	if (value.isValid())
	{
		ISDateTimeEdit::SetTime(value.toTime());
	}
	else
	{
		GetCheckEnable()->setChecked(false);
	}
}
//-----------------------------------------------------------------------------
QVariant ISTimeEdit::GetValue() const
{
	QVariant Time = ISDateTimeEdit::GetValue();
	if (Time.isValid())
	{
		QTime TimeValue = Time.toTime();
		TimeValue.setHMS(TimeValue.hour(), TimeValue.minute(), 0);
		return TimeValue;
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISTimeEdit::DateEnableChanged(int State)
{
	ISDateTimeEdit::DateEnableChanged(State);

	if (State == Qt::Checked)
	{
		ButtonTime->setEnabled(true);
	}
	else if (State == Qt::Unchecked)
	{
		ButtonTime->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISTimeEdit::SelectTime()
{
	QString TimeString = dynamic_cast<QAction*>(sender())->text();
	SetValue(QTime::fromString(TimeString, TIME_FORMAT_V1));
}
//-----------------------------------------------------------------------------
