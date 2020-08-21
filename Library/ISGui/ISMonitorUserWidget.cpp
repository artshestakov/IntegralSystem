#include "ISMonitorUserWidget.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISMonitorUserWidget::ISMonitorUserWidget(bool is_online, int user_id, const QString &user_name, QWidget *parent) : QFrame(parent)
{
	setFrameShape(QFrame::Box);
	setFrameShadow(QFrame::Plain);
	setAutoFillBackground(true);
	setCursor(CURSOR_POINTING_HAND);
	setContextMenuPolicy(Qt::ActionsContextMenu);
	setProperty("UserID", user_id);
	setProperty("UserName", user_name);

	QPalette Palette(palette());
	Palette.setColor(QPalette::Background, ISDefines::Gui::COLOR_MONITOR_USER);
	setPalette(Palette);

	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	setLayout(Layout);

	QLabel *LabelIcon = new QLabel(this);
	LabelIcon->setPixmap(BUFFER_ICONS(QString("UserMonitor.%1").arg(is_online ? "Online" : "Offline")).pixmap(ISDefines::Gui::SIZE_32_32));
	Layout->addWidget(LabelIcon, 0, Qt::AlignHCenter);

	QLabel *LabelUserName = new QLabel(this);
	LabelUserName->setText(QString(user_name).replace(SYMBOL_SPACE, "\n") + "\n" + (is_online ? LANG("MonitorActivity.MonitorUser.Online") : LANG("MonitorActivity.MonitorUser.Offline")));
	LabelUserName->setAlignment(Qt::AlignHCenter);
	LabelUserName->setWordWrap(true);
	Layout->addWidget(LabelUserName, 0, Qt::AlignHCenter);

	QAction *ActionUser = new QAction(this);
	ActionUser->setText(user_name);
	ActionUser->setIcon(BUFFER_ICONS("User"));
	ActionUser->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	connect(ActionUser, &QAction::triggered, this, &ISMonitorUserWidget::ShowUserCard);
	addAction(ActionUser);

	QAction *ActionProtocol = new QAction(this);
	ActionProtocol->setText(LANG("ProtocolUser"));
	ActionProtocol->setIcon(BUFFER_ICONS("Protocol"));
	connect(ActionProtocol, &QAction::triggered, this, &ISMonitorUserWidget::ShowProtocol);
	addAction(ActionProtocol);
}
//-----------------------------------------------------------------------------
ISMonitorUserWidget::~ISMonitorUserWidget()
{

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
