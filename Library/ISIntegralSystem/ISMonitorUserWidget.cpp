#include "StdAfx.h"
#include "ISMonitorUserWidget.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
ISMonitorUserWidget::ISMonitorUserWidget(bool is_online, int user_id, const QString &user_name, QWidget *parent) : QFrame(parent)
{
	UserID = user_id;
	UserName = user_name;

	setFrameShape(QFrame::Box);
	setFrameShadow(QFrame::Plain);
	setAutoFillBackground(true);
	setCursor(CURSOR_POINTING_HAND);
	setContextMenuPolicy(Qt::ActionsContextMenu);

	QPalette Palette(palette());
	Palette.setColor(QPalette::Background, COLOR_MONITOR_USER);
	setPalette(Palette);

	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(LAYOUT_MARGINS_5_PX);
	setLayout(Layout);

	QLabel *LabelIcon = new QLabel(this);
	Layout->addWidget(LabelIcon, 0, Qt::AlignHCenter);

	QLabel *LabelUserName = new QLabel(this);
	LabelUserName->setText(QString(user_name).replace(SYMBOL_SPACE, "\n"));
	LabelUserName->setAlignment(Qt::AlignHCenter);
	LabelUserName->setWordWrap(true);
	Layout->addWidget(LabelUserName, 0, Qt::AlignHCenter);

	QAction *ActionUser = new QAction(this);
	ActionUser->setText(UserName);
	ActionUser->setIcon(BUFFER_ICONS("User"));
	ActionUser->setFont(FONT_APPLICATION_BOLD);
	connect(ActionUser, &QAction::triggered, this, &ISMonitorUserWidget::ShowUserCard);
	addAction(ActionUser);

	QAction *ActionShowInfo = new QAction(this);
	ActionShowInfo->setText(LOCALIZATION("MonitorActivity.ActivityUser"));
	ActionShowInfo->setIcon(BUFFER_ICONS("ActivityUser"));
	connect(ActionShowInfo, &QAction::triggered, this, &ISMonitorUserWidget::ShowActivity);
	addAction(ActionShowInfo);

	QAction *ActionProtocol = new QAction(this);
	ActionProtocol->setText(LOCALIZATION("ProtocolUser"));
	ActionProtocol->setIcon(BUFFER_ICONS("Protocol"));
	connect(ActionProtocol, &QAction::triggered, this, &ISMonitorUserWidget::ShowProtocol);
	addAction(ActionProtocol);

	QAction *ActionDetails = new QAction(this);
	ActionDetails->setText(LOCALIZATION("Details"));
	ActionDetails->setIcon(BUFFER_ICONS("Information"));
	ActionDetails->setEnabled(is_online);
	connect(ActionDetails, &QAction::triggered, this, &ISMonitorUserWidget::ShowDetails);
	addAction(ActionDetails);

	QAction *ActionTerminate = new QAction(this);
	ActionTerminate->setText(LOCALIZATION("MonitorActivity.EndSession"));
	ActionTerminate->setIcon(BUFFER_ICONS("EndSessionUser"));
	connect(ActionTerminate, &QAction::triggered, this, &ISMonitorUserWidget::EndSession);
	addAction(ActionTerminate);

	QAction *ActionGetScreenshot = new QAction(this);
	ActionGetScreenshot->setText(LOCALIZATION("MonitorActivity.GetScreenshot"));
	ActionGetScreenshot->setIcon(BUFFER_ICONS("GetScreenshot"));
	ActionGetScreenshot->setEnabled(is_online);
	connect(ActionGetScreenshot, &QAction::triggered, this, &ISMonitorUserWidget::GetScreenshot);
	addAction(ActionGetScreenshot);

	QAction *ActionSendNotify = new QAction(this);
	ActionSendNotify->setText(LOCALIZATION("SendNotify"));
	ActionSendNotify->setIcon(BUFFER_ICONS("SendNotify"));
	ActionSendNotify->setEnabled(is_online);
	connect(ActionSendNotify, &QAction::triggered, this, &ISMonitorUserWidget::SendNotify);
	addAction(ActionSendNotify);

	if (is_online)
	{
		LabelIcon->setPixmap(BUFFER_ICONS("UserMonitor.Online").pixmap(SIZE_32_32));
		LabelUserName->setText(LabelUserName->text() + "\n(" + LOCALIZATION("MonitorActivity.MonitorUser.Online") + ")");
	}
	else
	{
		LabelIcon->setPixmap(BUFFER_ICONS("UserMonitor.Offline").pixmap(SIZE_32_32));
		LabelUserName->setText(LabelUserName->text() + "\n(" + LOCALIZATION("MonitorActivity.MonitorUser.Offline") + ")");
	}
}
//-----------------------------------------------------------------------------
ISMonitorUserWidget::~ISMonitorUserWidget()
{

}
//-----------------------------------------------------------------------------
int ISMonitorUserWidget::GetUserID() const
{
	return UserID;
}
//-----------------------------------------------------------------------------
QString ISMonitorUserWidget::GetUserName() const
{
	return UserName;
}
//-----------------------------------------------------------------------------
void ISMonitorUserWidget::enterEvent(QEvent *e)
{
	QWidget::enterEvent(e);
	setFrameShape(QFrame::WinPanel);
}
//-----------------------------------------------------------------------------
void ISMonitorUserWidget::leaveEvent(QEvent *e)
{
	QWidget::leaveEvent(e);
	setFrameShape(QFrame::Box);
}
//-----------------------------------------------------------------------------
void ISMonitorUserWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	QWidget::mouseDoubleClickEvent(e);
	emit ShowUserCard();
}
//-----------------------------------------------------------------------------
