#include "ISConnectionForm.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISConnectionForm::ISConnectionForm() : ISInterfaceDialogForm()
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

	EditDatabase = new ISLineEdit(this);
	EditDatabase->SetValue(CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE));
	FormLayout->addRow(LANG("DatabaseName") + ':', EditDatabase);

	EditProtocolInclude = new ISCheckEdit(this);
	EditProtocolInclude->SetValue(CONFIG_BOOL("Protocol/Include"));
	FormLayout->addRow(LANG("Protocol.Use"), EditProtocolInclude);

	EditProtocolPort = new ISIntegerEdit(this);
	EditProtocolPort->SetValue(CONFIG_INT("Protocol/Port"));
	FormLayout->addRow(LANG("Protocol.Port"), EditProtocolPort);

	QHBoxLayout *LayoutButtons = new QHBoxLayout();
	LayoutButtons->addStretch();
	GetMainLayout()->addLayout(LayoutButtons);

	ISPushButton *ButtonSave = new ISPushButton(BUFFER_ICONS("Save"), LANG("Save"), this);
	connect(ButtonSave, &ISPushButton::clicked, this, &ISConnectionForm::Save);
	LayoutButtons->addWidget(ButtonSave, 0, Qt::AlignRight);

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISConnectionForm::close);
	LayoutButtons->addWidget(ButtonClose);
}
//-----------------------------------------------------------------------------
ISConnectionForm::~ISConnectionForm()
{

}
//-----------------------------------------------------------------------------
void ISConnectionForm::AfterShowEvent()
{
	EditServer->SetFocus();
}
//-----------------------------------------------------------------------------
void ISConnectionForm::EnterClicked()
{
	Save();
}
//-----------------------------------------------------------------------------
void ISConnectionForm::Save()
{
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_SERVER, EditServer->GetValue());
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_PORT, EditPort->GetValue());
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_DATABASE, EditDatabase->GetValue());
	ISConfig::Instance().SetValue("Protocol/Include", EditProtocolInclude->GetValue());
	ISConfig::Instance().SetValue("Protocol/Port", EditProtocolPort->GetValue());
	close();
}
//-----------------------------------------------------------------------------
