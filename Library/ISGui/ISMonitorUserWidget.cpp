#include "ISMonitorUserWidget.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISMonitorUserWidget::ISMonitorUserWidget(bool is_online, int user_id, const QString &user_name, QWidget *parent)
	: QFrame(parent),
	UserID(user_id),
	UserName(user_name)
{
	setFrameShape(QFrame::Box);
	setFrameShadow(QFrame::Plain);
	setAutoFillBackground(true);
	setCursor(CURSOR_POINTING_HAND);
	setContextMenuPolicy(Qt::ActionsContextMenu);

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
	LabelUserName->setText(QString(user_name).replace(SYMBOL_SPACE, "\n") + "\n" + LANG("MonitorActivity.MonitorUser." + is_online ? "Online" : "Offline") + ')');
	LabelUserName->setAlignment(Qt::AlignHCenter);
	LabelUserName->setWordWrap(true);
	Layout->addWidget(LabelUserName, 0, Qt::AlignHCenter);

	QAction *ActionUser = new QAction(this);
	ActionUser->setText(UserName);
	ActionUser->setIcon(BUFFER_ICONS("User"));
	ActionUser->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	connect(ActionUser, &QAction::triggered, this, &ISMonitorUserWidget::ShowUserCard);
	addAction(ActionUser);

	QAction *ActionProtocol = new QAction(this);
	ActionProtocol->setText(LANG("ProtocolUser"));
	ActionProtocol->setIcon(BUFFER_ICONS("Protocol"));
	connect(ActionProtocol, &QAction::triggered, this, &ISMonitorUserWidget::ShowProtocol);
	addAction(ActionProtocol);

	QAction *ActionDetails = new QAction(this);
	ActionDetails->setText(LANG("Details"));
	ActionDetails->setIcon(BUFFER_ICONS("Information"));
	ActionDetails->setEnabled(is_online);
	connect(ActionDetails, &QAction::triggered, this, &ISMonitorUserWidget::ShowDetails);
	addAction(ActionDetails);
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
