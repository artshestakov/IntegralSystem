#include "StdAfx.h"
#include "ISAboutForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISConfig.h"
#include "ISMessageBox.h"
#include "ISFileDialog.h"
#include "ISMetaData.h"
#include "ISPushButton.h"
#include "ISButtonClose.h"
#include "ISTextEdit.h"
#include "ISQuery.h"
#include "ISLicense.h"
#include "ISVersion.h"
//-----------------------------------------------------------------------------
static QString QS_SHORTCUTS = PREPARE_QUERY("SELECT kbsc_shortcut, kbsc_description "
											"FROM _keyboardshortcuts "
											"ORDER BY kbsc_id");
//-----------------------------------------------------------------------------
ISAboutForm::ISAboutForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LOCALIZATION("AboutForm.AboutApplication"));
	setMinimumSize(SIZE_MAIN_WINDOW_MINIMUM);
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_5_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QPixmap PixmapLogo = BUFFER_PIXMAPS("Logo");
	QPixmap NewPixmap = PixmapLogo.scaled(QSize(200, 200), Qt::IgnoreAspectRatio, Qt::FastTransformation);
	
	QLabel *LabelImage = new QLabel(this);
	LabelImage->setPixmap(NewPixmap);
	Layout->addWidget(LabelImage);

	LayoutRight = new QVBoxLayout();
	LayoutRight->setContentsMargins(LAYOUT_MARGINS_10_PX);
	Layout->addLayout(LayoutRight);

	TabWidget = new QTabWidget(this);
	LayoutRight->addWidget(TabWidget);

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISAboutForm::close);
	LayoutRight->addWidget(ButtonClose, 0, Qt::AlignRight);

	CreateCommonTab();
	CreateContactsTab();
	CreateModuleTab();
	CreateDescriptionTab();
	CreateLicenseTab();
	CreateShortcuts();
	CreateOtherTab();
}
//-----------------------------------------------------------------------------
ISAboutForm::~ISAboutForm()
{

}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateCommonTab()
{
	QVBoxLayout *LayoutCommon = new QVBoxLayout();

	QWidget *TabCommon = new QWidget(TabWidget);
	TabCommon->setLayout(LayoutCommon);
	TabWidget->addTab(TabCommon, LOCALIZATION("AboutForm.Tab.Common"));

	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.ProductName"), LOCALIZATION("IntegralSystem"));
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.Cofiguration"), ISSystem::GetConfigurationName());
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.OS"), ISSystem::GetCurrentOSName());
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.Platform"), ISVersion::GetInstance().GetPlatform());
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.ProductVersion"), ISVersion::GetInstance().GetVersion());
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.ProductVersionRevision"), QString::number(ISVersion::GetInstance().GetRevision()));
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.ProductVersionBuild"), QString::number(ISVersion::GetInstance().GetBuild()));
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.ReleaseDate"), ISVersion::GetInstance().GetDate().toString(DATE_FORMAT_V2));
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.ReleaseTime"), ISVersion::GetInstance().GetTime().toString(TIME_FORMAT_V1));
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.ProductVersionHash"), ISVersion::GetInstance().GetHash());
	AddLabel(TabCommon, LOCALIZATION("AboutForm.Tab.Common.QtVersion"), qVersion());

	LayoutCommon->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateContactsTab()
{
	QVBoxLayout *LayoutContacts = new QVBoxLayout();

	QWidget *TabContacts = new QWidget(TabWidget);
	TabContacts->setLayout(LayoutContacts);
	TabWidget->addTab(TabContacts, LOCALIZATION("AboutForm.Tab.Contacts"));

	AddLabel(TabContacts, LOCALIZATION("AboutForm.Tab.Contacts.ProductAuthor"), LOCALIZATION("AboutForm.Tab.Contacts.ProductAuthor.FullName"));
    AddLabel(TabContacts, LOCALIZATION("AboutForm.Tab.Contacts.ProductAuthorPhone"), "+7 (903) 416-10-03");
	AddLabel(TabContacts, LOCALIZATION("AboutForm.Tab.Contacts.Telegram"), "@artem_shestakov");
    AddLabel(TabContacts, LOCALIZATION("AboutForm.Tab.Contacts.WhatsApp"), "+7 (903) 416-10-03");
	AddLabel(TabContacts, LOCALIZATION("AboutForm.Tab.Contacts.ProductAuthorMail"), "artemshestakov@outlook.com");
	AddLabel(TabContacts, LOCALIZATION("AboutForm.Tab.Contacts.Address"), LOCALIZATION("AboutForm.Tab.Contacts.Address.Text"));

	LayoutContacts->addStretch();

	ISPushButton *ButtonSaveInfo = new ISPushButton(TabContacts);
	ButtonSaveInfo->setText(LOCALIZATION("AboutForm.Tab.Contacts.SaveAuthorInformation"));
	ButtonSaveInfo->setIcon(BUFFER_ICONS("Save"));
	connect(ButtonSaveInfo, &ISPushButton::clicked, this, &ISAboutForm::SaveAuthorInfo);
	LayoutContacts->addWidget(ButtonSaveInfo, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateModuleTab()
{
	if (!ISMetaData::GetInstanse().GetInitialized())
	{
		return;
	}

	QVBoxLayout *LayoutModule = new QVBoxLayout();

	QWidget *TabModule = new QWidget(TabWidget);
	TabModule->setLayout(LayoutModule);
	TabWidget->addTab(TabModule, LOCALIZATION("AboutForm.Tab.Configuration"));

	AddLabel(TabModule, LOCALIZATION("AboutForm.Tab.Configuration.Name"), ISLicense::GetInstance().GetName());
	AddLabel(TabModule, LOCALIZATION("AboutForm.Tab.Configuration.LocalName"), ISLicense::GetInstance().GetLocalName());

	LayoutModule->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateDescriptionTab()
{
	QVBoxLayout *LayoutDescription = new QVBoxLayout();

	QWidget *TabDescription = new QWidget(TabWidget);
	TabDescription->setLayout(LayoutDescription);
	TabWidget->addTab(TabDescription, LOCALIZATION("AboutForm.Tab.Description"));

	ISTextEdit *TextEdit = new ISTextEdit(TabDescription);
	TextEdit->SetValue(LOCALIZATION("DescriptionApplication"));
	TextEdit->SetReadOnly(true);
	TabDescription->layout()->addWidget(TextEdit);
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateLicenseTab()
{
	QVBoxLayout *LayoutLicense = new QVBoxLayout();

	QWidget *TabLicense = new QWidget(TabWidget);
	TabLicense->setLayout(LayoutLicense);
	TabWidget->addTab(TabLicense, LOCALIZATION("AboutForm.Tab.License"));

	ISTextEdit *TextEdit = new ISTextEdit(TabLicense);
	TextEdit->SetReadOnly(true);
	TabLicense->layout()->addWidget(TextEdit);

	QFile FileLicense(":Licenses/IntegralSystem.txt");
	if (FileLicense.open(QIODevice::ReadOnly))
	{
		TextEdit->SetValue(QString::fromLocal8Bit(FileLicense.readAll()));
		FileLicense.close();
	}
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateShortcuts()
{
	if (!ISMetaData::GetInstanse().GetInitialized())
	{
		return;
	}

	QFormLayout *FormLayout = new QFormLayout();

	QWidget *TabShortcuts = new QWidget(TabWidget);
	TabShortcuts->setLayout(FormLayout);
	TabWidget->addTab(TabShortcuts, LOCALIZATION("AboutForm.Tab.Shortcuts"));

	ISQuery qSelect(QS_SHORTCUTS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString Shortcut = qSelect.ReadColumn("kbsc_shortcut").toString();
			QString Description = qSelect.ReadColumn("kbsc_description").toString();

			QLabel *Label = new QLabel(this);
			Label->setText(Shortcut + ":");
			Label->setFont(FONT_APPLICATION_BOLD);

			FormLayout->addRow(Label, new QLabel(Description, this));
		}
	}
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateOtherTab()
{
	QVBoxLayout *LayoutOther = new QVBoxLayout();

	QWidget *TabOther = new QWidget(TabWidget);
	TabOther->setLayout(LayoutOther);
	TabWidget->addTab(TabOther, LOCALIZATION("AboutForm.Tab.Other"));

	AddLabel(TabOther, LOCALIZATION("AboutForm.Tab.Other.SizeTemp"), ISSystem::GetSizeDir(APPLICATION_TEMP_PATH));
	AddLabel(TabOther, LOCALIZATION("AboutForm.Tab.Other.SizeLogger"), ISSystem::GetSizeDir(APPLICATION_LOGS_PATH));
	AddLabel(TabOther, LOCALIZATION("AboutForm.Tab.Other.ApplicationDir"), APPLICATION_DIR_PATH);

	LayoutOther->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutForm::AddLabel(QWidget *parent, const QString &LabelText, const QString &Text)
{
	QHBoxLayout *LayoutRow = new QHBoxLayout();
	LayoutRow->setContentsMargins(LAYOUT_MARGINS_NULL);

	QWidget *WidgetRow = new QWidget(parent);
	WidgetRow->setLayout(LayoutRow);

	QLabel *LabelRow = new QLabel(parent);
	LabelRow->setFont(FONT_APPLICATION_BOLD);
	LabelRow->setText(LabelText + ":");
	LayoutRow->addWidget(LabelRow);

	QLabel *Label = new QLabel(parent);
	Label->setFont(FONT_APPLICATION_BOLD);
	Label->setText(Text);
	LayoutRow->addWidget(Label);

	LayoutRow->addStretch();

	parent->layout()->addWidget(WidgetRow);
}
//-----------------------------------------------------------------------------
void ISAboutForm::SaveAuthorInfo()
{
	QString FilePath = ISFileDialog::GetSaveFileName(this, LOCALIZATION("File.Filter.Txt"));
	if (FilePath.length())
	{
		QFile File(FilePath);
		if (File.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QString String;
			String += LOCALIZATION("AboutForm.Tab.Contacts.ProductAuthor") + ": " + LOCALIZATION("AboutForm.Tab.Contacts.ProductAuthor.FullName") + "\n";
			String += LOCALIZATION("AboutForm.Tab.Contacts.ProductAuthorPhone") + ": +7 (903) 416-10-03\n";
			String += LOCALIZATION("AboutForm.Tab.Contacts.Telegram") + ": @artem_shestakov\n";
			String += LOCALIZATION("AboutForm.Tab.Contacts.WhatsApp") + ": +7 (903) 416-10-03\n";
			String += LOCALIZATION("AboutForm.Tab.Contacts.ProductAuthorMail") + ": artemshestakov@outlook.com";

			File.write(String.toUtf8());
			File.close();

			ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.FileSaved"));
		}
	}
}
//-----------------------------------------------------------------------------
