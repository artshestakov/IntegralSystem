#include "ISAboutForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
#include "ISFileDialog.h"
#include "ISButtons.h"
#include "ISButtons.h"
#include "ISTextEdit.h"
#include "ISQuery.h"
#include "ISVersion.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISQueryText.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
static QString QS_SHORTCUTS = PREPARE_QUERY("SELECT kbsc_shortcut, kbsc_description "
											"FROM _keyboardshortcuts "
											"ORDER BY kbsc_id");
//-----------------------------------------------------------------------------
ISAboutForm::ISAboutForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("AboutForm.AboutApplication"));
	setMinimumSize(DEFINES_GUI.SIZE_MAIN_WINDOW_MINIMUM);
	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_5_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QPixmap PixmapLogo = BUFFER_PIXMAPS("Logo");
	QPixmap NewPixmap = PixmapLogo.scaled(QSize(200, 200), Qt::IgnoreAspectRatio, Qt::FastTransformation);
	
	QLabel *LabelImage = new QLabel(this);
	LabelImage->setPixmap(NewPixmap);
	Layout->addWidget(LabelImage);

	LayoutRight = new QVBoxLayout();
	LayoutRight->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_10_PX);
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
	TabWidget->addTab(TabCommon, LANG("AboutForm.Tab.Common"));

	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ProductName"), LANG("IntegralSystem"));
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Cofiguration"), ISSystem::GetConfigurationName());
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.OS"), ISSystem::GetCurrentOSName());
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Platform"), ISVersion::GetInstance().GetPlatform());
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ProductVersion"), ISVersion::GetInstance().GetVersion());
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ProductVersionRevision"), QString::number(ISVersion::GetInstance().GetRevision()));
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ProductVersionBuild"), QString::number(ISVersion::GetInstance().GetBuild()));
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ReleaseDate"), ISVersion::GetInstance().GetDate().toString(DATE_FORMAT_V2));
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ReleaseTime"), ISVersion::GetInstance().GetTime().toString(TIME_FORMAT_V1));
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ProductVersionHash"), ISVersion::GetInstance().GetHash());
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.QtVersion"), qVersion());

	LayoutCommon->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateContactsTab()
{
	QVBoxLayout *LayoutContacts = new QVBoxLayout();

	QWidget *TabContacts = new QWidget(TabWidget);
	TabContacts->setLayout(LayoutContacts);
	TabWidget->addTab(TabContacts, LANG("AboutForm.Tab.Contacts"));

	AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.ProductAuthor"), LANG("AboutForm.Tab.Contacts.ProductAuthor.FullName"));
    AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.ProductAuthorPhone"), "+7 (918) 412-55-10");
	AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.Telegram"), "@artem_shestakov");
    AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.WhatsApp"), "+7 (918) 412-55-10");
	AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.ProductAuthorMail"), "art.shestakov@icloud.com");
	AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.Address"), LANG("AboutForm.Tab.Contacts.Address.Text"));

	LayoutContacts->addStretch();

	ISPushButton *ButtonSaveInfo = new ISPushButton(TabContacts);
	ButtonSaveInfo->setText(LANG("AboutForm.Tab.Contacts.SaveAuthorInformation"));
	ButtonSaveInfo->setIcon(BUFFER_ICONS("Save"));
	connect(ButtonSaveInfo, &ISPushButton::clicked, this, &ISAboutForm::SaveAuthorInfo);
	LayoutContacts->addWidget(ButtonSaveInfo, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateModuleTab()
{
	if (!ISObjects::GetInstance().IsInitialized())
	{
		return;
	}

	QVBoxLayout *LayoutModule = new QVBoxLayout();

	QWidget *TabModule = new QWidget(TabWidget);
	TabModule->setLayout(LayoutModule);
	TabWidget->addTab(TabModule, LANG("AboutForm.Tab.Configuration"));

	AddLabel(TabModule, LANG("AboutForm.Tab.Configuration.Name"), ISObjects::GetInstance().GetInfo().Name);
	AddLabel(TabModule, LANG("AboutForm.Tab.Configuration.LocalName"), ISObjects::GetInstance().GetInfo().LocalName);
	AddLabel(TabModule, LANG("AboutForm.Tab.Configuration.UID"), ISObjects::GetInstance().GetInfo().UID);

	LayoutModule->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateDescriptionTab()
{
	QVBoxLayout *LayoutDescription = new QVBoxLayout();

	QWidget *TabDescription = new QWidget(TabWidget);
	TabDescription->setLayout(LayoutDescription);
	TabWidget->addTab(TabDescription, LANG("AboutForm.Tab.Description"));

	ISTextEdit *TextEdit = new ISTextEdit(TabDescription);
	TextEdit->SetValue(LANG("DescriptionApplication"));
	TextEdit->SetReadOnly(true);
	TabDescription->layout()->addWidget(TextEdit);
}
//-----------------------------------------------------------------------------
void ISAboutForm::CreateLicenseTab()
{
	QVBoxLayout *LayoutLicense = new QVBoxLayout();

	QWidget *TabLicense = new QWidget(TabWidget);
	TabLicense->setLayout(LayoutLicense);
	TabWidget->addTab(TabLicense, LANG("AboutForm.Tab.License"));

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
	if (!ISObjects::GetInstance().IsInitialized())
	{
		return;
	}

	QFormLayout *FormLayout = new QFormLayout();

	QWidget *TabShortcuts = new QWidget(TabWidget);
	TabShortcuts->setLayout(FormLayout);
	TabWidget->addTab(TabShortcuts, LANG("AboutForm.Tab.Shortcuts"));

	ISQuery qSelect(QS_SHORTCUTS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString Shortcut = qSelect.ReadColumn("kbsc_shortcut").toString();
			QString Description = qSelect.ReadColumn("kbsc_description").toString();

			QLabel *Label = new QLabel(this);
			Label->setText(Shortcut + ':');
			Label->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);

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
	TabWidget->addTab(TabOther, LANG("AboutForm.Tab.Other"));

	AddLabel(TabOther, LANG("AboutForm.Tab.Other.SizeTemp"), ISSystem::GetSizeDir(DEFINES_CORE.PATH_TEMP_DIR));
	AddLabel(TabOther, LANG("AboutForm.Tab.Other.SizeLogger"), ISSystem::GetSizeDir(DEFINES_CORE.PATH_LOGS_DIR));
	AddLabel(TabOther, LANG("AboutForm.Tab.Other.ApplicationDir"), DEFINES_CORE.PATH_APPLICATION_DIR);

	LayoutOther->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutForm::AddLabel(QWidget *parent, const QString &LabelText, const QString &Text)
{
	QHBoxLayout *LayoutRow = new QHBoxLayout();
	LayoutRow->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_NULL);

	QWidget *WidgetRow = new QWidget(parent);
	WidgetRow->setLayout(LayoutRow);

	QLabel *LabelRow = new QLabel(parent);
	LabelRow->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
	LabelRow->setText(LabelText + ':');
	LayoutRow->addWidget(LabelRow);

	QLabel *Label = new QLabel(parent);
	Label->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
	Label->setText(Text);
	LayoutRow->addWidget(Label);

	LayoutRow->addStretch();

	parent->layout()->addWidget(WidgetRow);
}
//-----------------------------------------------------------------------------
void ISAboutForm::SaveAuthorInfo()
{
	QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.Txt"));
	if (FilePath.length())
	{
		QFile File(FilePath);
		if (File.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QString String;
			String += LANG("AboutForm.Tab.Contacts.ProductAuthor") + ": " + LANG("AboutForm.Tab.Contacts.ProductAuthor.FullName") + "\n";
			String += LANG("AboutForm.Tab.Contacts.ProductAuthorPhone") + ": +7 (918) 412-55-10\n";
			String += LANG("AboutForm.Tab.Contacts.Telegram") + ": @artem_shestakov\n";
			String += LANG("AboutForm.Tab.Contacts.WhatsApp") + ": +7 (918) 412-55-10\n";
			String += LANG("AboutForm.Tab.Contacts.ProductAuthorMail") + ": art.shestakov@icloud.com";

			File.write(String.toUtf8());
			File.close();

			ISMessageBox::ShowInformation(this, LANG("Message.Information.FileSaved"));
		}
	}
}
//-----------------------------------------------------------------------------
