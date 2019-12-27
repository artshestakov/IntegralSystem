#include "StdAfx.h"
#include "ISConnectionForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISConnectionForm::ISConnectionForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LOCALIZATION("ConnectionSettings"));

	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);
	ForbidResize();

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	QLabel *LabelServer = new QLabel(this);
	LabelServer->setText(LOCALIZATION("Server") + ":");
	LabelServer->setToolTip(LOCALIZATION("ConnectionForm.Hint.Server"));
	LabelServer->setCursor(CURSOR_WHATS_THIS);

	EditServer = new ISIPAddressEdit(this);
	EditServer->SetValue(CONFIG_VALUE(CONST_CONFIG_CONNECTION_SERVER));
	FormLayout->addRow(LabelServer, EditServer);

	QLabel *LabelPort = new QLabel(this);
	LabelPort->setText(LOCALIZATION("Port") + ":");
	LabelPort->setToolTip(LOCALIZATION("ConnectionForm.Hint.Port"));
	LabelPort->setCursor(CURSOR_WHATS_THIS);

	EditPort = new ISIntegerEdit(this);
	EditPort->SetValue(CONFIG_VALUE(CONST_CONFIG_CONNECTION_PORT));
	FormLayout->addRow(LabelPort, EditPort);

	QLabel *LabelDatabase = new QLabel(this);
	LabelDatabase->setText(LOCALIZATION("DatabaseName") + ":");
	LabelDatabase->setToolTip(LOCALIZATION("ConnectionForm.Hint.Database"));
	LabelDatabase->setCursor(CURSOR_WHATS_THIS);

	EditDatabase = new ISDatabaseEdit(this);
	EditDatabase->SetCurrentText(CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE));
	EditDatabase->SetPlaceholderText(LOCALIZATION("EnterDatabaseName"));
	FormLayout->addRow(LabelDatabase, EditDatabase);

	QLabel *LabelLanguage = new QLabel(this);
	LabelLanguage->setText(LOCALIZATION("Language") + ":");
	LabelLanguage->setCursor(CURSOR_WHATS_THIS);

	EditLanguage = new ISLanguageEdit(this);
	EditLanguage->SetValue(ISConfig::GetInstance().GetValueString(CONST_CONFIG_OTHER_LANGUAGE));
	FormLayout->addRow(LabelLanguage, EditLanguage);

	QFormLayout *FormLayoutInput = new QFormLayout();

	GroupBoxInput = new QGroupBox(LOCALIZATION("AutoInput"), this);
	GroupBoxInput->setCheckable(true);
	GroupBoxInput->setChecked(CONFIG_BOOL(CONST_CONFIG_AUTOINPUT_INCLUDED));
	GroupBoxInput->setLayout(FormLayoutInput);
	GetMainLayout()->addWidget(GroupBoxInput);

	EditLogin = new ISLineEdit(GroupBoxInput);
	EditLogin->SetValue(CONFIG_VALUE(CONST_CONFIG_AUTOINPUT_LOGIN));
	FormLayoutInput->addRow(LOCALIZATION("Login") + ":", EditLogin);

	EditPassword = new ISPasswordEdit(GroupBoxInput);
	EditPassword->SetValue(CONFIG_VALUE(CONST_CONFIG_AUTOINPUT_PASSWORD));
	FormLayoutInput->addRow(LOCALIZATION("Password") + ":", EditPassword);

	CheckAutoboot = new ISCheckEdit(this);
	CheckAutoboot->SetText(LOCALIZATION("AutobootStartup"));
	CheckAutoboot->SetToolTip(LOCALIZATION("AutobootStartup.ToolTip"));
	CheckAutoboot->SetValue(CONFIG_VALUE(CONST_CONFIG_OTHER_AUTOBOOT));
	GetMainLayout()->addWidget(CheckAutoboot);

	ButtonSave = new ISPushButton(this);
	ButtonSave->setText(LOCALIZATION("Save"));
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
	if (!CheckFields())
	{
		return;
	}

	ISConfig::GetInstance().SetValue(CONST_CONFIG_CONNECTION_SERVER, EditServer->GetValue());
	ISConfig::GetInstance().SetValue(CONST_CONFIG_CONNECTION_PORT, EditPort->GetValue());
	ISConfig::GetInstance().SetValue(CONST_CONFIG_CONNECTION_DATABASE, EditDatabase->GetCurrentText());
	ISConfig::GetInstance().SetValue(CONST_CONFIG_OTHER_AUTOBOOT, CheckAutoboot->GetValue());
	ISConfig::GetInstance().SetValue(CONST_CONFIG_OTHER_LANGUAGE, EditLanguage->GetValue());
	ISConfig::GetInstance().SetValue(CONST_CONFIG_AUTOINPUT_INCLUDED, GroupBoxInput->isChecked());
	ISConfig::GetInstance().SetValue(CONST_CONFIG_AUTOINPUT_LOGIN, EditLogin->GetValue());
	ISConfig::GetInstance().SetValue(CONST_CONFIG_AUTOINPUT_PASSWORD, EditPassword->GetValue());

#ifdef Q_OS_WIN32
	QSettings Settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	if (CheckAutoboot->GetValue().toBool())
	{
		Settings.setValue(APPLICATION_NAME, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
		Settings.sync();
	}
	else
	{
		Settings.remove(APPLICATION_NAME);
	}
#endif

	emit AutiInputChanged(EditLogin->GetValue().toString(), EditPassword->GetValue().toString());
	SaveDatabaseName();
	close();
}
//-----------------------------------------------------------------------------
bool ISConnectionForm::CheckFields()
{
	if (EditServer->GetValue().toString().isEmpty())
	{
		ISMessageBox::ShowCritical(this, LOCALIZATION("Message.Error.Field.NullValue").arg(LOCALIZATION("Server")));
		return false;
	}
	else if (EditPort->GetValue().toString().isEmpty())
	{
		ISMessageBox::ShowCritical(this, LOCALIZATION("Message.Error.Field.NullValue").arg(LOCALIZATION("Port")));
		return false;
	}
	else if (EditDatabase->GetCurrentText().isEmpty())
	{
		ISMessageBox::ShowCritical(this, LOCALIZATION("Message.Error.Field.NullValue").arg(LOCALIZATION("DatabaseName")));
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
void ISConnectionForm::SaveDatabaseName()
{
	QFile FileTemp(APPLICATION_TEMP_PATH + "/ConnectDatabases");
	if (FileTemp.open(QIODevice::ReadWrite))
	{
		QString DatabaseName = EditDatabase->GetCurrentText();
		QString Content = FileTemp.readAll();
		QStringList StringList = Content.split(";");
		if (!StringList.contains(DatabaseName))
		{
			StringList.append(DatabaseName);
			for (const QString &String : StringList)
			{
				if (String.length())
				{
					FileTemp.write(String.toUtf8() + ";");
				}
			}
		}
		FileTemp.close();
	}
}
//-----------------------------------------------------------------------------
