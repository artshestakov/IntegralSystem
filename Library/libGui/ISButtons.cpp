#include "ISButtons.h"
#include "ISConstants.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISLocalization.h"
#include "ISNotifyRecipient.h"
#include "ISGui.h"
#include "ISNotificationsForm.h"
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent) : QPushButton(Icon, Text, parent)
{
	setToolTip(ToolTip);
	setStyleSheet(STYLE_SHEET("ISPushButton"));
	setMinimumWidth(ISPUSHBUTTON_MINIMUM_WIDTH);
	setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QIcon &Icon, const QString &Text, QWidget *parent) : ISPushButton(Icon, Text, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QString &Text, QWidget *parent) : ISPushButton(QIcon(), Text, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QIcon &Icon, QWidget *parent) : ISPushButton(Icon, QString(), QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(QWidget *parent) : ISPushButton(QIcon(), QString(), QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::~ISPushButton()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISButtonClear::ISButtonClear(QWidget *parent) : ISPushButton(parent)
{
	setFixedSize(ISDefines::Gui::SIZE_22_22);
	setCursor(CURSOR_POINTING_HAND);
	setToolTip(LANG("Clear.Field"));
	setFocusPolicy(Qt::NoFocus);
	setIcon(BUFFER_ICONS("ButtonClear"));
	setFlat(true);
}
//-----------------------------------------------------------------------------
ISButtonClear::~ISButtonClear()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISButtonClose::ISButtonClose(QWidget *parent) : ISPushButton(parent)
{
	setText(LANG("Close"));
	setIcon(BUFFER_ICONS("Close"));
}
//-----------------------------------------------------------------------------
ISButtonClose::~ISButtonClose()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent) : ISPushButton(Icon, Text, ToolTip, parent)
{
	setFixedSize(ISDefines::Gui::SIZE_22_22);
	setCursor(CURSOR_POINTING_HAND);
}
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(const QIcon &Icon, const QString &Text, QWidget *parent) : ISServiceButton(Icon, Text, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(const QString &Text, QWidget *parent) : ISServiceButton(QIcon(), Text, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(QWidget *parent) : ISServiceButton(QIcon(), QString(), QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISServiceButton::~ISServiceButton()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISToolButton::ISToolButton(QWidget *parent) : QToolButton(parent)
{
	setStyleSheet(STYLE_SHEET("ISPushButton"));
	setMinimumWidth(ISPUSHBUTTON_MINIMUM_WIDTH);
	setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
}
//-----------------------------------------------------------------------------
ISToolButton::~ISToolButton()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISButtonMainMenu::ISButtonMainMenu(QWidget *parent)
	: QToolButton(parent),
	IconDefault(BUFFER_ICONS("MainPanel.Menu")),
	IconActive(BUFFER_ICONS("MainPanel.Menu.Active"))
{
	setText(LANG("MainMenu"));
	setFont(ISDefines::Gui::FONT_TAHOMA_10);
	setAutoRaise(true);
	setCheckable(true);
	setIcon(IconDefault);
	setIconSize(ISDefines::Gui::SIZE_24_24);
	setCursor(CURSOR_POINTING_HAND);
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	setStyleSheet(STYLE_SHEET("QToolButtonMainMenu"));
}
//-----------------------------------------------------------------------------
ISButtonMainMenu::~ISButtonMainMenu()
{

}
//-----------------------------------------------------------------------------
void ISButtonMainMenu::enterEvent(QEvent *e)
{
	QToolButton::enterEvent(e);
	setIcon(IconActive);
}
//-----------------------------------------------------------------------------
void ISButtonMainMenu::leaveEvent(QEvent *e)
{
	QToolButton::leaveEvent(e);
	setIcon(IconDefault);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISCalculatorButton::ISCalculatorButton(QWidget *parent) : ISPushButton(parent)
{
	setFixedSize(40, 40);
}
//-----------------------------------------------------------------------------
ISCalculatorButton::~ISCalculatorButton()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISButtonNotify::ISButtonNotify(QWidget *parent)
	: QToolButton(parent),
	NotifyCount(0),
	IconNull(BUFFER_ICONS("MainPanel.Notifications.Null")),
	IconNullActive(BUFFER_ICONS("MainPanel.Notifications.Null.Active")),
	IconNew(BUFFER_ICONS("MainPanel.Notifications.New")),
	IconNewActive(BUFFER_ICONS("MainPanel.Notifications.New.Active")),
	IsBlink(false),
	StyleWhite(STYLE_SHEET("ISButtonNotify.White")),
	StyleBlink(STYLE_SHEET("ISButtonNotify.Blink"))
{
	connect(&ISNotifyRecipient::GetInstance(), static_cast<void(ISNotifyRecipient::*)(const QVariantMap &)>(&ISNotifyRecipient::Notify), this, &ISButtonNotify::NotifyEvent);

	setIcon(IconNull);
	setToolTip(LANG("Notifications"));
	setAutoRaise(true);
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	setCursor(CURSOR_POINTING_HAND);
	setIconSize(ISDefines::Gui::SIZE_24_24);
	setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
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
	setIcon(NotifyCount ? IconNew : IconNull);
	setText(NotifyCount ? QString("+(%1)").arg(NotifyCount) : QString());
	setToolTip(NotifyCount ? LANG("Notifications.ToolTip").arg(NotifyCount) : LANG("Notifications"));
	NotifyCount ? Timer->start() : Timer->stop();
}
//-----------------------------------------------------------------------------
void ISButtonNotify::enterEvent(QEvent *e)
{
	setIcon(text().isEmpty() ? IconNullActive : IconNewActive);
}
//-----------------------------------------------------------------------------
void ISButtonNotify::leaveEvent(QEvent *e)
{
	setIcon(text().isEmpty() ? IconNull : IconNew);
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
	setStyleSheet(IsBlink ? StyleWhite : StyleBlink);
	IsBlink = !IsBlink;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISButtonFile::ISButtonFile(QWidget *parent) : ISPushButton(parent)
{
	setAcceptDrops(true);
}
//-----------------------------------------------------------------------------
ISButtonFile::~ISButtonFile()
{

}
//-----------------------------------------------------------------------------
void ISButtonFile::dragEnterEvent(QDragEnterEvent *e)
{
	const QMimeData *MimeData = e->mimeData();
	if (MimeData->hasFormat("text/uri-list"))
	{
		if (MimeData->urls().count() > 1) //Если пользователь перетаскивает более одного файла
		{
			return;
		}
		else
		{
			e->acceptProposedAction();
		}
	}
}
//-----------------------------------------------------------------------------
void ISButtonFile::dropEvent(QDropEvent *e)
{
	const QMimeData* MimeData = e->mimeData();
	if (MimeData->hasUrls())
	{
		emit DragAndDropFile(MimeData->urls()[0].toString(QUrl::PreferLocalFile));
	}
}
//-----------------------------------------------------------------------------
void ISButtonFile::dragMoveEvent(QDragMoveEvent *e)
{
	if (e->mimeData()->hasUrls())
	{
		e->acceptProposedAction();
	}
}
//-----------------------------------------------------------------------------
