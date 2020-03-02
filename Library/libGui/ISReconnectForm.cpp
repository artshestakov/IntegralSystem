#include "ISReconnectForm.h"
#include "ISDefinesGui.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISMetaUser.h"
#include "ISBuffer.h"
#include "ISButtons.h"
#include "ISMessageBox.h"
#include "ISControls.h"
#include "ISDatabase.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISReconnectForm::ISReconnectForm() : ISInterfaceDialogForm()
{
	CurrentAttemp = 0;

	setWindowTitle(LANG("ReconnectToDatabase"));
	setWindowIcon(BUFFER_ICONS("Reconnect"));
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	setCursor(CURSOR_BUSY);

	ForbidResize();

	QPixmap Pixmap = BUFFER_PIXMAPS("BannerText");

	QLabel *LabelBannerText = new QLabel(this);
	LabelBannerText->setPixmap(Pixmap.scaled(Pixmap.width() / 2, Pixmap.height() / 2));
	GetMainLayout()->addWidget(LabelBannerText);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	Layout = new QVBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	GetMainLayout()->addLayout(Layout);

	ProgressBar = new QProgressBar(this);
	ProgressBar->setValue(0);
	ProgressBar->setRange(0, 0);
	ProgressBar->setTextVisible(false);
	Layout->addWidget(ProgressBar);

	QLabel *LabelConnectionLost = new QLabel(this);
	LabelConnectionLost->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LabelConnectionLost->setText(LANG("ConnectionDatabaseLost"));
	Layout->addWidget(LabelConnectionLost, 0, Qt::AlignLeft);

	AddLabel(LANG("CausesReconnect") + ':');
	AddLabel(ISDefines::Core::SYMBOL_CIRCLE + SYMBOL_SPACE + LANG("Causes.Server.NotLocalNet") + ';');
	AddLabel(ISDefines::Core::SYMBOL_CIRCLE + SYMBOL_SPACE + LANG("Causes.Server.NotEntered") + ';');
	AddLabel(ISDefines::Core::SYMBOL_CIRCLE + SYMBOL_SPACE + LANG("Causes.PatchCord.NotConnected") + ';');
	AddLabel(ISDefines::Core::SYMBOL_CIRCLE + SYMBOL_SPACE + LANG("Causes.Service.NotEntered") + ';');

	Layout->addWidget(ISControls::CreateHorizontalLine(this));

	QHBoxLayout *LayoutReconnect = new QHBoxLayout();
	LayoutReconnect->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	Layout->addLayout(LayoutReconnect);

	QLabel *LabelStatus = new QLabel(this);
	LabelStatus->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LabelStatus->setText(LANG("Status.Current") + ':');
	LayoutReconnect->addWidget(LabelStatus);

	LabelCurrentStatus = new QLabel(this);
	LabelCurrentStatus->setText(LANG("ReconnectText").arg('1') + "...");
	LayoutReconnect->addWidget(LabelCurrentStatus);

	LayoutReconnect->addStretch();

	Layout->addWidget(ISControls::CreateHorizontalLine(this));

	QLabel *LabelReconnectStartDate = new QLabel(this);
	LabelReconnectStartDate->setText(LANG("ReconnectWaitFor") + ": " + QDateTime::currentDateTime().toString(DATE_TIME_FORMAT_V3));
	Layout->addWidget(LabelReconnectStartDate);

	QHBoxLayout *LayoutButtons = new QHBoxLayout();
	LayoutButtons->addStretch();
	Layout->addLayout(LayoutButtons);

	ISPushButton *ButtonRestart = new ISPushButton(this);
	ButtonRestart->setText(LANG("RestartApplication"));
	connect(ButtonRestart, &ISPushButton::clicked, this, &ISReconnectForm::Restart);
	LayoutButtons->addWidget(ButtonRestart);

	ISPushButton *ButtonExit = new ISPushButton(this);
	ButtonExit->setText(LANG("Exit"));
	connect(ButtonExit, &ISPushButton::clicked, this, &ISReconnectForm::Exit);
	LayoutButtons->addWidget(ButtonExit);

	Timer = new QTimer(this);
	Timer->setInterval(FIVE_SECOND_TO_MILLISECOND);
	connect(Timer, &QTimer::timeout, this, &ISReconnectForm::TimerTick);
}
//-----------------------------------------------------------------------------
ISReconnectForm::~ISReconnectForm()
{

}
//-----------------------------------------------------------------------------
ISReconnectForm& ISReconnectForm::GetInstance()
{
	static ISReconnectForm ReconnectForm;
	return ReconnectForm;
}
//-----------------------------------------------------------------------------
void ISReconnectForm::StartReconnect(bool &Connect)
{
	QEventLoop EventLoop;
	Timer->start();

	connect(this, &ISReconnectForm::ConnectInstalled, &EventLoop, &QEventLoop::quit);
	EventLoop.exec();

	Connect = Connected;
}
//-----------------------------------------------------------------------------
bool ISReconnectForm::Connect()
{
	return ISDatabase::GetInstance().ConnectToDefaultDB(ISMetaUser::GetInstance().GetData()->Login, ISMetaUser::GetInstance().GetData()->Password, QString());
}
//-----------------------------------------------------------------------------
void ISReconnectForm::Restart()
{
	ISGui::RestartApplication();
}
//-----------------------------------------------------------------------------
void ISReconnectForm::Exit()
{
	Timer->stop();
	ISGui::ExitApplication();
}
//-----------------------------------------------------------------------------
void ISReconnectForm::Hide()
{
	CurrentAttemp = 0;
	LabelCurrentStatus->clear();
	hide();
}
//-----------------------------------------------------------------------------
void ISReconnectForm::AddLabel(const QString &Text)
{
	QLabel *Label = new QLabel(this);
	Label->setText(Text);
	Label->setWordWrap(true);
	Layout->addWidget(Label);
}
//-----------------------------------------------------------------------------
void ISReconnectForm::TimerTick()
{
	++CurrentAttemp;
	LabelCurrentStatus->setText(LANG("ReconnectText").arg(CurrentAttemp) + "...");

	Connected = Connect();
	if (Connected)
	{
		Timer->stop();
		emit ConnectInstalled();
		Hide();
	}
}
//-----------------------------------------------------------------------------
void ISReconnectForm::EscapeClicked()
{
	Timer->stop();
	ISMessageBox::ShowWarning(this, LANG("ProgramWillExit"));
	Exit();
}
//-----------------------------------------------------------------------------
