#include "ISReconnectForm.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISDefinesGui.h"
#include "ISConfig.h"
#include "ISTcpConnector.h"
#include "ISButtons.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISReconnectForm::ISReconnectForm()
	: ISInterfaceDialogForm(true),
	Attempts(0)
{
	setWindowTitle(LANG("ReconnectForm.Title"));
	setWindowIcon(BUFFER_ICONS("Update"));
	setCursor(CURSOR_BUSY);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	resize(300, 165);

	QLabel *LabelInfo = new QLabel(LANG("ReconnectForm.Info"), this);
	LabelInfo->setWordWrap(true);
	GetMainLayout()->addWidget(LabelInfo);

	LabelAttempts = new QLabel(LANG("ReconnectForm.Attempts").arg(Attempts), this);
	GetMainLayout()->addWidget(LabelAttempts);

	QProgressBar *ProgressBar = new QProgressBar(this);
	ProgressBar->setValue(0);
	ProgressBar->setRange(0, 0);
	ProgressBar->setTextVisible(false);
	GetMainLayout()->addWidget(ProgressBar);

	Timer = new QTimer(this);
	connect(Timer, &QTimer::timeout, this, &ISReconnectForm::Timeout);
	Timer->setInterval(CARAT_RECONNECT_TIMEOUT);
	Timer->setSingleShot(true);
	Timer->start();

	GetMainLayout()->addStretch();

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->addStretch();
	GetMainLayout()->addLayout(LayoutBottom);

	ISPushButton *ButtonExit = new ISPushButton(BUFFER_ICONS("Exit"), LANG("ExitApplication"), this);
	connect(ButtonExit, &ISPushButton::clicked, this, &ISReconnectForm::close);
	LayoutBottom->addWidget(ButtonExit);
}
//-----------------------------------------------------------------------------
ISReconnectForm::~ISReconnectForm()
{

}
//-----------------------------------------------------------------------------
void ISReconnectForm::Timeout()
{
	if (ISTcpConnector::Instance().Connect(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT("Protocol/Port")))
	{
		SetResult(true);
		close();
	}
	else
	{
		LabelAttempts->setText(LANG("ReconnectForm.Attempts").arg(++Attempts));
		Timer->start();
	}
}
//-----------------------------------------------------------------------------
