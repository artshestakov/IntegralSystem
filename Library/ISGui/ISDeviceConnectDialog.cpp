#include "ISDeviceConnectDialog.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
ISDeviceConnectDialog::ISDeviceConnectDialog() : ISInterfaceDialogForm()
{
	resize(440, 220);
	ForbidResize();
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	setWindowTitle(LANG("ISDeviceConnectDialog.Title"));
	setWindowIcon(BUFFER_ICONS("USBDevice"));

	QLabel *LabelTitle = new QLabel(LANG("ISDeviceConnectDialog.LabelInfo"), this);
	LabelTitle->setFont(ISDefines::Gui::FONT_TAHOMA_11);
	LabelTitle->setWordWrap(true);
	GetMainLayout()->addWidget(LabelTitle);

	GetMainLayout()->addStretch();

	QProgressBar *ProgressBar = new QProgressBar(this);
	ProgressBar->setRange(0, 0);
	ProgressBar->setTextVisible(false);
	GetMainLayout()->addWidget(ProgressBar);

	ISPushButton *ButtonClose = new ISPushButton(LANG("ISDeviceConnectDialog.Cancel"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISDeviceConnectDialog::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISDeviceConnectDialog::~ISDeviceConnectDialog()
{
	
}
//-----------------------------------------------------------------------------
void ISDeviceConnectDialog::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
}
//-----------------------------------------------------------------------------
