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

	EditUpdateDir = new ISPathEditDir(this);
	EditUpdateDir->SetValue(CONFIG_STRING(CONST_CONFIG_CONNECTION_UPDATE_DIR));
	FormLayout->addRow(LANG("UpdateDir") + ':', EditUpdateDir);

	EditProtocolUse = new ISCheckEdit(this);
	EditProtocolUse->SetValue(CONFIG_BOOL("Protocol/Use"));
	FormLayout->addRow("Use", EditProtocolUse);

	EditProtocolAuth = new ISCheckEdit(this);
	EditProtocolAuth->SetValue(CONFIG_BOOL("Protocol/Auth"));
	FormLayout->addRow("Auth", EditProtocolAuth);

	EditProtocolHost = new ISLineEdit(this);
	EditProtocolHost->SetValue(CONFIG_STRING("Protocol/Host"));
	FormLayout->addRow("Host", EditProtocolHost);

	EditProtocolPort = new ISIntegerEdit(this);
	EditProtocolPort->SetValue(CONFIG_INT("Protocol/Port"));
	FormLayout->addRow("Port", EditProtocolPort);

	ISPushButton *ButtonSave = new ISPushButton(this);
	ButtonSave->setText(LANG("Save"));
	ButtonSave->setIcon(BUFFER_ICONS("Save"));
	connect(ButtonSave, &ISPushButton::clicked, this, &ISConnectionForm::SaveSettings);
	GetMainLayout()->addWidget(ButtonSave, 0, Qt::AlignRight);
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
	SaveSettings();
}
//-----------------------------------------------------------------------------
void ISConnectionForm::SaveSettings()
{
	if (CheckFields())
	{
		ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_SERVER, EditServer->GetValue());
		ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_PORT, EditPort->GetValue());
		ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_DATABASE, EditDatabase->GetValue());
		ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_UPDATE_DIR, EditUpdateDir->GetValue());
		ISConfig::Instance().SetValue("Protocol/Use", EditProtocolUse->GetValue());
		ISConfig::Instance().SetValue("Protocol/Auth", EditProtocolAuth->GetValue());
		ISConfig::Instance().SetValue("Protocol/Host", EditProtocolHost->GetValue());
		ISConfig::Instance().SetValue("Protocol/Port", EditProtocolPort->GetValue());
		close();
	}
}
//-----------------------------------------------------------------------------
bool ISConnectionForm::CheckFields()
{
	if (EditServer->GetValue().toString().isEmpty())
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.Field.NullValue").arg(LANG("Server")));
		return false;
	}
	else if (EditPort->GetValue().toString().isEmpty())
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.Field.NullValue").arg(LANG("Port")));
		return false;
	}
	else if (EditDatabase->GetValue().toString().isEmpty())
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.Field.NullValue").arg(LANG("DatabaseName")));
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
