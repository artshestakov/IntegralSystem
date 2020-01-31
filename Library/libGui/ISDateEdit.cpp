#include "StdAfx.h"
#include "ISDateEdit.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
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
	ActionToday->setFont(FONT_APPLICATION_BOLD);
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
		SetActionsIcon(DATE_TODAY);
	}
}
//-----------------------------------------------------------------------------
void ISDateEdit::BeforeYesterday()
{
	SetValue(DATE_BEFORE_YESTERDAY);
}
//-----------------------------------------------------------------------------
void ISDateEdit::Yesterday()
{
	SetValue(DATE_YESTERDAY);
}
//-----------------------------------------------------------------------------
void ISDateEdit::Today()
{
	SetValue(DATE_TODAY);
}
//-----------------------------------------------------------------------------
void ISDateEdit::Tomorrow()
{
	SetValue(DATE_TOMORROW);
}
//-----------------------------------------------------------------------------
void ISDateEdit::AfterTomorrow()
{
	SetValue(DATE_AFTER_TOMORROW);
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
		if (Date == DATE_BEFORE_YESTERDAY)
		{
			ActionBeforeYesterday->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
		else if (Date == DATE_YESTERDAY)
		{
			ActionYesterday->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
		else if (Date == DATE_TODAY)
		{
			ActionToday->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
		else if (Date == DATE_TOMORROW)
		{
			ActionTomorrow->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
		else if (Date == DATE_AFTER_TOMORROW)
		{
			ActionAfterTomorrow->setIcon(BUFFER_ICONS("Apply.Blue"));
		}
	}
	else
	{

	}
}
//-----------------------------------------------------------------------------
