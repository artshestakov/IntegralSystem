#include "ISCalendarPopup.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISCalendarPopup::ISCalendarPopup(QWidget *parent) : ISCalendarWidget(parent)
{
    setWindowFlags(Qt::Popup);

    ButtonToday = new QToolButton(this);
    ButtonToday->setAutoRaise(true);
    ButtonToday->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
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
void ISCalendarPopup::showEvent(QShowEvent *ShowEvent)
{
    ISCalendarWidget::showEvent(ShowEvent);
    ButtonToday->setText(LANG_FMT("ISCalendarPopup.Today", QDate::currentDate().toString(FORMAT_DATE_V2).toStdString().c_str()));
}
//-----------------------------------------------------------------------------
