#include "ISDateTimeEdit.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISDateTimeEdit::ISDateTimeEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	VisibleCalendar = true;

	CheckEnable = new QCheckBox(this);
	CheckEnable->setCursor(CURSOR_POINTING_HAND);
	CheckEnable->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
	CheckEnable->setToolTip(LANG("ActivateDeactivateField"));
	connect(CheckEnable, &QCheckBox::stateChanged, this, &ISDateTimeEdit::DateEnableChanged);
	AddWidgetToLeft(CheckEnable);

	DateTimeEdit = new ISQDateTimeEdit(this);
	DateTimeEdit->setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	DateTimeEdit->setWrapping(true);
	DateTimeEdit->setAccelerated(true);
	DateTimeEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
	DateTimeEdit->setDisplayFormat(DATE_TIME_FORMAT_V1);
	DateTimeEdit->setSpecialValueText(LANG("NotIndicated"));
	DateTimeEdit->setAlignment(Qt::AlignCenter);
	AddWidgetEdit(DateTimeEdit, this);

	ButtonCalendar = new QToolButton(this);
	ButtonCalendar->setToolTip(LANG("ShowCalendar") + "...");
	ButtonCalendar->setIcon(BUFFER_ICONS("Calendar"));
	ButtonCalendar->setCursor(CURSOR_POINTING_HAND);
	ButtonCalendar->setFocusPolicy(Qt::NoFocus);
	connect(ButtonCalendar, &QToolButton::clicked, this, &ISDateTimeEdit::ShowCalendar);
	AddWidgetToRight(ButtonCalendar);

	CalendarWidget = new ISCalendarPopup(this);
	CalendarWidget->setVisible(false);
	connect(CalendarWidget, &ISCalendarWidget::clicked, this, &ISDateTimeEdit::SetValue);
	connect(CalendarWidget, &ISCalendarWidget::Hide, this, &ISDateTimeEdit::HideCalendar);

	AnimationCalendar = new QPropertyAnimation(CalendarWidget, "windowOpacity", this);
	AnimationCalendar->setDuration(500);

	CheckEnable->stateChanged(Qt::Unchecked); //ѕо умолчанию поле не активно
	connect(DateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &ISDateTimeEdit::ValueChanged); //ƒанный коннект должен быть об€зательно после предыдущей строчки, иначе поле будет иметь статус "»зменено" сразу после создани€

	SetFixedWidth(170);
}
//-----------------------------------------------------------------------------
ISDateTimeEdit::~ISDateTimeEdit()
{

}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetValue(const QVariant &value)
{
	if (value.toDateTime().isValid())
	{
		CheckEnable->setChecked(true);
		DateTimeEdit->setDateTime(value.toDateTime());
		HideCalendar();
	}
	else
	{
		CheckEnable->setChecked(false);
	}
}
//-----------------------------------------------------------------------------
QVariant ISDateTimeEdit::GetValue() const
{
	if (CheckEnable->isChecked())
	{
		return DateTimeEdit->dateTime();
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::Clear()
{
	ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetVisibleClear(bool Visible)
{
	ISFieldEditBase::SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetDisplayFormat(const QString &DisplayFormat)
{
	DateTimeEdit->setDisplayFormat(DisplayFormat);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetVisibleCheck(bool Visible)
{
	CheckEnable->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetCheckEnable(Qt::CheckState Check)
{
	CheckEnable->setCheckState(Check);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetMinimumDate(const QDate &Date)
{
	DateTimeEdit->setMinimumDate(Date);
	CalendarWidget->setMinimumDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetMaximumDate(const QDate &Date)
{
	DateTimeEdit->setMaximumDate(Date);
	CalendarWidget->setMaximumDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetMinimumDateTime(const QDateTime &DateTime)
{
	DateTimeEdit->setMinimumDateTime(DateTime);
	CalendarWidget->setMinimumDate(DateTime.date());
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetMaximumDateTime(const QDateTime &DateTime)
{
	DateTimeEdit->setMaximumDateTime(DateTime);
	CalendarWidget->setMaximumDate(DateTime.date());
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetReadOnly(bool ReadOnly)
{
	DateTimeEdit->setReadOnly(ReadOnly);
	CheckEnable->setVisible(!ReadOnly);

	if (VisibleCalendar)
	{
		ButtonCalendar->setVisible(!ReadOnly);
	}
}
//-----------------------------------------------------------------------------
QCheckBox* ISDateTimeEdit::GetCheckEnable()
{
	return CheckEnable;
}
//-----------------------------------------------------------------------------
QToolButton* ISDateTimeEdit::GetButtonCalendar()
{
	return ButtonCalendar;
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetDate(const QDate &Date)
{
	CheckEnable->setChecked(true);
	DateTimeEdit->setDate(Date);
	HideCalendar();
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetTime(const QTime &Time)
{
	CheckEnable->setChecked(true);
	DateTimeEdit->setTime(Time);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::DateEnableChanged(int State)
{
	if (State == Qt::Checked)
	{
		DateTimeEdit->setSpecialValueText(QString());
		DateTimeEdit->setDateTime(QDateTime::currentDateTime());
		DateTimeEdit->setEnabled(true);
		DateTimeEdit->setSelectedSection(SelectSection);
		ButtonCalendar->setEnabled(true);
		SetFocus();
	}
	else
	{
		DateTimeEdit->setSpecialValueText(LANG("NotIndicated"));
		DateTimeEdit->setDateTime(DateTimeEdit->minimumDateTime());
		DateTimeEdit->setEnabled(false);
		ButtonCalendar->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::ShowCalendar()
{
	QPoint Point = DateTimeEdit->mapToGlobal(QPoint());
	Point.setY(Point.y() + DateTimeEdit->height());

	AnimationCalendar->setStartValue(0.0);
	AnimationCalendar->setEndValue(1.0);

	CalendarWidget->move(Point);
	CalendarWidget->setSelectedDate(GetValue().toDate());
	CalendarWidget->setWindowOpacity(0.0);
	CalendarWidget->show();
	CalendarWidget->setFocus();
	AnimationCalendar->start();
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::HideCalendar()
{
	AnimationCalendar->setStartValue(1.0);
	AnimationCalendar->setEndValue(0.0);

	SetFocus();
	DateTimeEdit->setSelectedSection(ISQDateTimeEdit::HourSection);
	AnimationCalendar->start();
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetVisibleCalendar(bool visible)
{
	VisibleCalendar = visible;

	ButtonCalendar->setVisible(VisibleCalendar);
	CalendarWidget->setVisible(VisibleCalendar);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetSelectSection(ISQDateTimeEdit::Section select_section)
{
	SelectSection = select_section;
}
//-----------------------------------------------------------------------------
