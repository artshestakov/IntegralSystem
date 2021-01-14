#include "ISAboutDialog.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISFileDialog.h"
#include "ISButtons.h"
#include "ISVersionInfo.h"
#include "ISConstants.h"
#include "ISObjects.h"
#include "ISConfig.h"
#include "ISFieldEdits.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISAboutDialog::ISAboutDialog() : ISInterfaceDialogForm()
{
	setWindowTitle(LANG("AboutForm.AboutApplication"));
	setMinimumSize(ISDefines::Gui::SIZE_MAIN_WINDOW_MINIMUM);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QPixmap PixmapLogo = BUFFER_PIXMAPS("Logo");
	QPixmap NewPixmap = PixmapLogo.scaled(QSize(200, 200), Qt::IgnoreAspectRatio, Qt::FastTransformation);
	
	QLabel *LabelImage = new QLabel(this);
	LabelImage->setPixmap(NewPixmap);
	Layout->addWidget(LabelImage);

	LayoutRight = new QVBoxLayout();
	Layout->addLayout(LayoutRight);

	TabWidget = new QTabWidget(this);
	LayoutRight->addWidget(TabWidget);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutBottom);

	ISPushButton *ButtonFeedback = new ISPushButton(BUFFER_ICONS("Feedback"), LANG("AboutForm.Feedback"), this);
	connect(ButtonFeedback, &ISPushButton::clicked, this, &ISAboutDialog::Feedback);
	LayoutBottom->addWidget(ButtonFeedback);

	LayoutBottom->addStretch();

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISAboutDialog::close);
	LayoutBottom->addWidget(ButtonClose);

	CreateCommonTab();
	CreateContactsTab();
	CreateModuleTab();
	CreateDescriptionTab();
	CreateLicenseTab();
	CreateOtherTab();
}
//-----------------------------------------------------------------------------
ISAboutDialog::~ISAboutDialog()
{

}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateCommonTab()
{
	QVBoxLayout *LayoutCommon = new QVBoxLayout();

	QWidget *TabCommon = new QWidget(TabWidget);
	TabCommon->setLayout(LayoutCommon);
	TabWidget->addTab(TabCommon, LANG("AboutForm.Tab.Common"));

	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ProductName"), QCoreApplication::applicationName());
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Cofiguration"), ISVersionInfo::Instance().Info.Configuration);
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Platform"), ISVersionInfo::Instance().Info.Platform);
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Version"), QString::number(ISVersionInfo::Instance().Info.Version));
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ReleaseDateTime"),
		ISVersionInfo::Instance().Info.Date + " " + LANG("In") + " " + ISVersionInfo::Instance().Info.Time);
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Hash"), ISVersionInfo::Instance().Info.Hash);
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Branch"), ISVersionInfo::Instance().Info.BranchName);
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.QtVersion"), qVersion());
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Server"), CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER));
	AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Port"), CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT));

	LayoutCommon->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateContactsTab()
{
	QVBoxLayout *LayoutContacts = new QVBoxLayout();

	QWidget *TabContacts = new QWidget(TabWidget);
	TabContacts->setLayout(LayoutContacts);
	TabWidget->addTab(TabContacts, LANG("AboutForm.Tab.Contacts"));

	AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.ProductAuthor"), LANG("AboutForm.Tab.Contacts.ProductAuthor.FullName"));
    AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.ProductAuthorPhone"), "+7 (918) 412-55-10");
	AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.Telegram"), "@artem_shestakov");
	AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.ProductAuthorMail"), "art.shestakov@icloud.com");
	AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.Address"), LANG("AboutForm.Tab.Contacts.Address.Text"));

	LayoutContacts->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateModuleTab()
{
	if (ISVersionInfo::Instance().ConfigurationInfo.Name.isEmpty())
	{
		return;
	}

	QVBoxLayout *LayoutModule = new QVBoxLayout();

	QWidget *TabModule = new QWidget(TabWidget);
	TabModule->setLayout(LayoutModule);
	TabWidget->addTab(TabModule, LANG("AboutForm.Tab.Configuration"));

	AddLabel(TabModule, LANG("AboutForm.Tab.Configuration.Name"), ISVersionInfo::Instance().ConfigurationInfo.Name);
	AddLabel(TabModule, LANG("AboutForm.Tab.Configuration.LocalName"), ISVersionInfo::Instance().ConfigurationInfo.LocalName);
	AddLabel(TabModule, LANG("AboutForm.Tab.Configuration.UID"), ISVersionInfo::Instance().ConfigurationInfo.UID);

	LayoutModule->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateDescriptionTab()
{
	QVBoxLayout *LayoutDescription = new QVBoxLayout();

	QWidget *TabDescription = new QWidget(TabWidget);
	TabDescription->setLayout(LayoutDescription);
	TabWidget->addTab(TabDescription, LANG("AboutForm.Tab.Description"));

	ISTextEdit *TextEdit = new ISTextEdit(TabDescription);
	TextEdit->SetValue(LANG("DescriptionApplication"));
	TextEdit->SetReadOnly(true);
	TextEdit->SetSizePolicyVertical(QSizePolicy::Minimum);
	TabDescription->layout()->addWidget(TextEdit);
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateLicenseTab()
{
	QVBoxLayout *LayoutLicense = new QVBoxLayout();

	QWidget *TabLicense = new QWidget(TabWidget);
	TabLicense->setLayout(LayoutLicense);
	TabWidget->addTab(TabLicense, LANG("AboutForm.Tab.License"));

	ISTextEdit *TextEdit = new ISTextEdit(TabLicense);
	TextEdit->SetReadOnly(true);
	TextEdit->SetSizePolicyVertical(QSizePolicy::Minimum);
	TabLicense->layout()->addWidget(TextEdit);

	QFile FileLicense(":Licenses/IntegralSystem.txt");
	if (FileLicense.open(QIODevice::ReadOnly))
	{
		TextEdit->SetValue(QString::fromLocal8Bit(FileLicense.readAll()));
		FileLicense.close();
	}
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateOtherTab()
{
	QVBoxLayout *LayoutOther = new QVBoxLayout();

	QWidget *TabOther = new QWidget(TabWidget);
	TabOther->setLayout(LayoutOther);
	TabWidget->addTab(TabOther, LANG("AboutForm.Tab.Other"));

	AddLabel(TabOther, LANG("AboutForm.Tab.Other.ApplicationDir"), QCoreApplication::applicationDirPath());

	LayoutOther->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutDialog::AddLabel(QWidget *parent, const QString &LabelText, const QString &Text)
{
	QHBoxLayout *LayoutRow = new QHBoxLayout();
	LayoutRow->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *WidgetRow = new QWidget(parent);
	WidgetRow->setLayout(LayoutRow);

	QLabel *LabelRow = new QLabel(parent);
	LabelRow->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LabelRow->setText(LabelText + ':');
	LayoutRow->addWidget(LabelRow);

	QLabel *Label = new QLabel(parent);
	Label->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	Label->setText(Text);
	LayoutRow->addWidget(Label);

	LayoutRow->addStretch();

	parent->layout()->addWidget(WidgetRow);
}
//-----------------------------------------------------------------------------
void ISAboutDialog::Feedback()
{
	ISGui::SetWaitGlobalCursor(true);
	bool Opened = QDesktopServices::openUrl(QUrl(LINK_FEEDBACK));
	ISGui::SetWaitGlobalCursor(false);
	if (!Opened)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotOpenFeedbackLink"), LINK_FEEDBACK);
	}
}
//-----------------------------------------------------------------------------
