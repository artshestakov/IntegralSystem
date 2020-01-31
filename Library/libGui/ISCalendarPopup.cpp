#include "StdAfx.h"
#include "ISCalendarPopup.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISCalendarPopup::ISCalendarPopup(QWidget *parent) : ISCalendarWidget(parent)
{
	setWindowFlags(Qt::Popup);

	QToolButton *ButtonToday = new QToolButton(this);
	ButtonToday->setAutoRaise(true);
	ButtonToday->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ButtonToday->setText(LANG("Today") + ": " + DATE_TODAY.toString(DATE_FORMAT_V2));
	ButtonToday->setToolTip(LANG("ClickFromSelectCurrentDay"));
	ButtonToday->setIcon(BUFFER_ICONS("Today"));
	ButtonToday->setCursor(CURSOR_POINTING_HAND);
	ButtonToday->setFont(FONT_APPLICATION_BOLD);
	connect(ButtonToday, &QToolButton::clicked, this, &ISCalendarWidget::Today);
	GetLayout()->addWidget(ButtonToday, 0, Qt::AlignHCenter);

	adjustSize();
	resize(width(), height() + ButtonToday->height());
}
//-----------------------------------------------------------------------------
ISCalendarPopup::~ISCalendarPopup()
{

}
//-----------------------------------------------------------------------------
