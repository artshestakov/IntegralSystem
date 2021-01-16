#include "ISConnectDialog.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISConnectDialog::ISConnectDialog() : ISInterfaceDialogForm()
{
	setWindowTitle(LANG("ConnectionSettings"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	resize(650, 300);

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	EditServer = new ISLineEdit(this);
	EditServer->SetValue(CONFIG_VALUE(CONST_CONFIG_CONNECTION_SERVER));
	FormLayout->addRow(LANG("Server") + ':', EditServer);

	EditPort = new ISIntegerEdit(this);
	EditPort->SetValue(CONFIG_VALUE(CONST_CONFIG_CONNECTION_PORT));
	FormLayout->addRow(LANG("Port") + ':', EditPort);

	QHBoxLayout *LayoutButtons = new QHBoxLayout();
	LayoutButtons->addStretch();
	GetMainLayout()->addLayout(LayoutButtons);

	ISPushButton *ButtonSave = new ISPushButton(BUFFER_ICONS("Save"), LANG("Save"), this);
	connect(ButtonSave, &ISPushButton::clicked, this, &ISConnectDialog::Save);
	LayoutButtons->addWidget(ButtonSave, 0, Qt::AlignRight);

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISConnectDialog::close);
	LayoutButtons->addWidget(ButtonClose);
}
//-----------------------------------------------------------------------------
ISConnectDialog::~ISConnectDialog()
{

}
//-----------------------------------------------------------------------------
void ISConnectDialog::AfterShowEvent()
{
	EditServer->SetFocus();
}
//-----------------------------------------------------------------------------
void ISConnectDialog::EnterClicked()
{
	Save();
}
//-----------------------------------------------------------------------------
void ISConnectDialog::Save()
{
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_SERVER, EditServer->GetValue());
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_PORT, EditPort->GetValue());
	close();
}
//-----------------------------------------------------------------------------
