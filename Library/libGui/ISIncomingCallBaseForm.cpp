#include "ISIncomingCallBaseForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISButtons.h"
#include "ISButtons.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISSettings.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISIncomingCallBaseForm::ISIncomingCallBaseForm(const QVariantMap &payload, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	Payload = payload;

	setWindowTitle(LANG("IncomingCall"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	ForbidResize();

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	QLabel *LabelIcon = new QLabel(this);
	LabelIcon->setMovie(BUFFER_ANIMATION("IncomingCall", this));
	LabelIcon->movie()->start();
	LayoutTitle->addWidget(LabelIcon);

	ISPushButton *ButtonCard = new ISPushButton(this);
	ButtonCard->setText(LANG("Card"));
	ButtonCard->setToolTip(LANG("Card"));
	ButtonCard->setIcon(BUFFER_ICONS("Document"));
	connect(ButtonCard, &ISPushButton::clicked, this, &ISIncomingCallBaseForm::OpenCard);
	LayoutTitle->addWidget(ButtonCard);

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISIncomingCallBaseForm::close);
	LayoutTitle->addWidget(ButtonClose);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	if (SETTING_BOOL(CONST_UID_SETTING_TELEPHONY_AUTOCLOSEINCOMINGFORM))
	{
		SettingTimeout = SETTING_INT(CONST_UID_SETTING_TELEPHONY_AUTOCLOSEINCOMINGFORMTIMEOUT);
		CurrentTimeout = 0;

		LabelTimeout = new QLabel(this);
		LabelTimeout->setText(LANG("AutoCloseIncomingForm").arg(SettingTimeout));
		GetMainLayout()->addWidget(LabelTimeout);

		GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

		QTimer *Timer = new QTimer(this);
		connect(Timer, &QTimer::timeout, this, &ISIncomingCallBaseForm::Timeout);
		Timer->start(1000);
	}

	GroupBox = new QGroupBox(this);
	GroupBox->setTitle(LANG("InfoAboutCall"));
	GroupBox->setLayout(new QFormLayout());
	GroupBox->setVisible(false);
	GetMainLayout()->addWidget(GroupBox);
}
//-----------------------------------------------------------------------------
ISIncomingCallBaseForm::~ISIncomingCallBaseForm()
{

}
//-----------------------------------------------------------------------------
void ISIncomingCallBaseForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();

	QString PositionForm = SETTING_STRING(CONST_UID_SETTING_VIEW_INCOMINGFORMPOSITION);
	if (PositionForm == "LeftUp")
	{
		ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_LeftUp);
	}
	else if (PositionForm == "LeftBottom")
	{
		ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_LeftBottom);
	}
	else if (PositionForm == "RightUp")
	{
		ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_RightUp);
	}
	else if (PositionForm == "RightBottom")
	{
		ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_RightBottom);
	}
	else if (PositionForm == "Center")
	{
		ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
	}

	FlashingStart(700, ISDefines::Gui::COLOR_FLASH_INCOMING_FORM);
}
//-----------------------------------------------------------------------------
void ISIncomingCallBaseForm::Timeout()
{
	--SettingTimeout;
	LabelTimeout->setText(LANG("AutoCloseIncomingForm").arg(SettingTimeout));
	if (SettingTimeout < 1)
	{
		SetResult(true);
		close();
	}
}
//-----------------------------------------------------------------------------
void ISIncomingCallBaseForm::AddLabel(const QString &LabelName, const QString &LabelText)
{
	if (!GroupBox->isVisible())
	{
		GroupBox->setVisible(true);
	}

	dynamic_cast<QFormLayout*>(GroupBox->layout())->addRow(LabelName, new QLabel(LabelText, GroupBox));
}
//-----------------------------------------------------------------------------
void ISIncomingCallBaseForm::OpenCard()
{
	
}
//-----------------------------------------------------------------------------
