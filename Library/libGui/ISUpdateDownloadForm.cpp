#include "ISUpdateDownloadForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISMessageBox.h"
#include "ISUpdate.h"
#include "ISVersion.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_DATA_ID = PREPARE_QUERY("SELECT dfdt_id "
										  "FROM _distfilesdata "
										  "WHERE dfdt_file = :FileID "
										  "ORDER BY dfdt_id");
//-----------------------------------------------------------------------------
static QString QS_DATA = PREPARE_QUERY("SELECT dfdt_data "
									   "FROM _distfilesdata "
									   "WHERE dfdt_id = :DataID "
									   "ORDER BY dfdt_id");
//-----------------------------------------------------------------------------
ISUpdateDownloadForm::ISUpdateDownloadForm(int file_id, const QString &file_name, const QString &version, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	FileID = file_id;
	FileName = file_name;

	setWindowTitle(LANG("UpdateApplication"));
	setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	resize(DEFINES_GUI.SIZE_500_90);
	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_10_PX);
	ForbidResize();

	QLabel *LabelInformation = new QLabel(this);
	LabelInformation->setWordWrap(true);
	LabelInformation->setText(LANG("LoadingUpdateText") + "\n\n" + LANG("CurrentVersion") + ": " + ISVersion::GetInstance().GetVersion() + "\n" + LANG("NewVersion") + ": " + version);
	GetMainLayout()->addWidget(LabelInformation);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	QLabel *Label = new QLabel(this);
	Label->setText(LANG("Action") + ':');
	LayoutTitle->addWidget(Label);

	LabelStatus = new QLabel(this);
	LayoutTitle->addWidget(LabelStatus);

	LayoutTitle->addStretch();

	ProgressBar = new QProgressBar(this);
	GetMainLayout()->addWidget(ProgressBar);

	ButtonDialog = new ISButtonDialog(this, LANG("StartDownloading"), LANG("Annulment"));
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISUpdateDownloadForm::Start);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISUpdateDownloadForm::Close);
	GetMainLayout()->addWidget(ButtonDialog);

	connect(&ISUpdate::GetInstance(), &ISUpdate::Message, this, &ISUpdateDownloadForm::SetStatusText);
	connect(&ISUpdate::GetInstance(), &ISUpdate::ProgressMaximum, ProgressBar, &QProgressBar::setMaximum);
	connect(&ISUpdate::GetInstance(), &ISUpdate::ProgressValue, ProgressBar, &QProgressBar::setValue);
}
//-----------------------------------------------------------------------------
ISUpdateDownloadForm::~ISUpdateDownloadForm()
{
	
}
//-----------------------------------------------------------------------------
void ISUpdateDownloadForm::EnterClicked()
{
	Start();
}
//-----------------------------------------------------------------------------
void ISUpdateDownloadForm::SetStatusText(const QString &Text)
{
	LabelStatus->setText(Text);
	ISGui::RepaintWidget(LabelStatus);
}
//-----------------------------------------------------------------------------
void ISUpdateDownloadForm::Start()
{
	ButtonDialog->SetApplyEnabled(false);
	
	bool Loaded = ISUpdate::GetInstance().LoadUpdate(FileID, FileName);
	if (Loaded)
	{
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.NotCloseUpdating"));
	}

	SetResult(Loaded);
	close();
}
//-----------------------------------------------------------------------------
void ISUpdateDownloadForm::Close()
{
	if (ISUpdate::GetInstance().GetLoading())
	{
		ISUpdate::GetInstance().SetStop(true);
	}
	else
	{
		close();
	}
}
//-----------------------------------------------------------------------------
