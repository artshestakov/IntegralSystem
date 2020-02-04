#include "ISButtonNotify.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISNotifySender.h"
#include "ISNotifyRecipient.h"
#include "ISGui.h"
#include "ISNotificationsForm.h"
//-----------------------------------------------------------------------------
ISButtonNotify::ISButtonNotify(QWidget *parent) : QToolButton(parent)
{
	NotifyCount = 0;
	IconNull = BUFFER_ICONS("MainPanel.Notifications.Null");
	IconNullActive = BUFFER_ICONS("MainPanel.Notifications.Null.Active");
	IconNew = BUFFER_ICONS("MainPanel.Notifications.New");
	IconNewActive = BUFFER_ICONS("MainPanel.Notifications.New.Active");
	IsBlink = false;
	StyleWhite = STYLE_SHEET("ISButtonNotify.White");
	StyleBlink = STYLE_SHEET("ISButtonNotify.Blink");

	connect(&ISNotifyRecipient::GetInstance(), static_cast<void(ISNotifyRecipient::*)(const QVariantMap &)>(&ISNotifyRecipient::Notify), this, &ISButtonNotify::NotifyEvent);

	setIcon(IconNull);
	setToolTip(LANG("Notifications"));
	setAutoRaise(true);
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	setCursor(CURSOR_POINTING_HAND);
	setIconSize(SIZE_24_24);
	setFont(FONT_APPLICATION_BOLD);
	setStyleSheet(STYLE_SHEET("ISButtonNotify.White"));
	connect(this, &ISButtonNotify::clicked, this, &ISButtonNotify::Clicked);

	Timer = new QTimer(this);
	Timer->setInterval(1000);
	connect(Timer, &QTimer::timeout, this, &ISButtonNotify::Timeout);
}
//-----------------------------------------------------------------------------
ISButtonNotify::~ISButtonNotify()
{

}
//-----------------------------------------------------------------------------
void ISButtonNotify::SetCountNotify(int Count)
{
	NotifyCount = Count;
	if (NotifyCount)
	{
		setIcon(IconNew);
		setText(QString("+(%1)").arg(NotifyCount));
		setToolTip(LANG("Notifications.ToolTip") + ": " + QString::number(NotifyCount));
		Timer->start();
	}
	else
	{
		setIcon(IconNull);
		setText(QString());
		setToolTip(LANG("Notifications"));
		Timer->stop();
	}
}
//-----------------------------------------------------------------------------
void ISButtonNotify::enterEvent(QEvent *e)
{
	if (text().length())
	{
		setIcon(IconNewActive);
	}
	else
	{
		setIcon(IconNullActive);
	}
}
//-----------------------------------------------------------------------------
void ISButtonNotify::leaveEvent(QEvent *e)
{
	if (text().length())
	{
		setIcon(IconNew);
	}
	else
	{
		setIcon(IconNull);
	}
}
//-----------------------------------------------------------------------------
void ISButtonNotify::Clicked()
{
	ISGui::SetWaitGlobalCursor(true);

	ISNotificationsForm *NotificationsForm = new ISNotificationsForm(this);
	NotificationsForm->adjustSize();
	NotificationsForm->move(QPoint(mapToGlobal(QPoint()).x(), mapToGlobal(QPoint()).y() + height()));
	connect(NotificationsForm, &ISNotificationsForm::CountChanged, this, &ISButtonNotify::SetCountNotify);
	NotificationsForm->ShowAnimated(false, 400);

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISButtonNotify::NotifyEvent(const QVariantMap &VariantMap)
{
	if (VariantMap.value("Save").toBool())
	{
		SetCountNotify(NotifyCount + 1);
	}
}
//-----------------------------------------------------------------------------
void ISButtonNotify::Timeout()
{
	if (IsBlink)
	{
		setStyleSheet(StyleWhite);
	}
	else
	{
		setStyleSheet(StyleBlink);
	}

	IsBlink = !IsBlink;
}
//-----------------------------------------------------------------------------
