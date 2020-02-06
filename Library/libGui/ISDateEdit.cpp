#include "ISDateEdit.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISDateEdit::ISDateEdit(QWidget *parent) : ISDateTimeEdit(parent)
{
	SetDisplayFormat(DATE_FORMAT_V4);
	SetSelectSection(ISQDateTimeEdit::DaySection);
	
	GetButtonCalendar()->setPopupMode(QToolButton::MenuButtonPopup);

	QMenu *Menu = new QMenu(GetButtonCalendar());
	GetButtonCalendar()->setMenu(Menu);

	ActionBeforeYesterday = new QAction(Menu);
	ActionBeforeYesterday->setText(LANG("BeforeYesterday"));
	ActionBeforeYesterday->setToolTip(LANG("BeforeYesterday"));
	connect(ActionBeforeYesterday, &QAction::triggered, this, &ISDateEdit::BeforeYesterday);
	Menu->addAction(ActionBeforeYesterday);

	ActionYesterday = new QAction(Menu);
	ActionYesterday->setText(LANG("Yesterday"));
	ActionYesterday->setToolTip(LANG("Yesterday"));
	connect(ActionYesterday, &QAction::triggered, this, &ISDateEdit::Yesterday);
	Menu->addAction(ActionYesterday);

	ActionToday = new QAction(Menu);
	ActionToday->setText(LANG("Today"));
	ActionToday->setToolTip(LANG("Today"));
	ActionToday->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
	ActionToday->setIcon(BUFFER_ICONS("Apply.Blue"));
	connect(ActionToday, &QAction::triggered, this, &ISDateEdit::Today);
	Menu->addAction(ActionToday);
	
	ActionTomorrow = new QAction(Menu);
	ActionTomorrow->setText(LANG("Tomorrow"));
	ActionTomorrow->setToolTip(LANG("Tomorrow"));
	connect(ActionTomorrow, &QAction::triggered, this, &ISDateEdit::Tomorrow);
	Menu->addAction(ActionTomorrow);

	ActionAfterTomorrow = new QAction(Menu);
	ActionAfterTomorrow->setText(LANG("AfterTomorrow"));
	ActionAfterTomorrow->setToolTip(LANG("AfterTomorrow"));
	connect(ActionAfterTomorrow, &QAction::triggered, this, &ISDateEdit::AfterTomorrow);
	Menu->addAction(ActionAfterTomorrow);
}
//-----------------------------------------------------------------------------
ISDateEdit::~ISDateEdit()
{

}
//-----------------------------------------------------------------------------
void ISDateEdit::SetValue(const QVariant &value)
{
	if (value.isValid())
	{
		ISDateTimeEdit::SetDate(value.toDate());
	}
	else
	{
		GetCheckEnable()->setChecked(false);
	}

	SetActionsIcon(value.toDate());
}
//-----------------------------------------------------------------------------
QVariant ISDateEdit::GetValue() const
{
	QVariant DateValue = ISDateTimeEdit::GetValue();
	if (DateValue.isValid())
	{
		return DateValue.toDate();
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISDateEdit::DateEnableChanged(int State)
{
	ISDateTimeEdit::DateEnableChanged(State);

	if (State == Qt::Unchecked)
	{
		SetActionsIcon(QDate::currentDate());
	}
}
//-----------------------------------------------------------------------------
void ISDateEdit::BeforeYesterday()
{
	SetValue(QDate::currentDate().addDays(-2));
}
//-----------------------------------------------------------------------------
void ISDateEdit::Yesterday()
{
	SetValue(QDate::currentDate().addDays(-1));
}
//-----------------------------------------------------------------------------
void ISDateEdit::Today()
{
	SetValue(QDate::currentDate());
}
//-----------------------------------------------------------------------------
void ISDateEdit::Tomorrow()
{
	SetValue(QDate::currentDate().addDays(1));
}
//-----------------------------------------------------------------------------
void ISDateEdit::AfterTomorrow()
{
	SetValue(QDate::currentDate().addDays(2));
}
//-----------------------------------------------------------------------------
void ISDateEdit::SetActionsIcon(const QDate &Date)
{
	ActionBeforeYesterday->setIcon(QIcon());
	ActionYesterday->setIcon(QIcon());
	ActionToday->setIcon(QIcon());
	ActionTomorrow->setIcon(QIcon());
	ActionAfterTomorrow->setIcon(QIcon());

	if (Date.isValid())
	{
		if (Date == QDate::currentDate().addDays(-2))
		{
			ActionBeforeYesterday->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
		else if (Date == QDate::currentDate().addDays(-1))
		{
			ActionYesterday->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
		else if (Date == QDate::currentDate())
		{
			ActionToday->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
		else if (Date == QDate::currentDate().addDays(1))
		{
			ActionTomorrow->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
		else if (Date == QDate::currentDate().addDays(2))
		{
			ActionAfterTomorrow->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
	}
	else
	{

	}
}
//-----------------------------------------------------------------------------
