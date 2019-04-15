#include "StdAfx.h"
#include "ISUpdateDownloadForm.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
#include "ISDebug.h"
#include "ISCountingTime.h"
#include "ISUpdate.h"
#include "ISVersion.h"
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

	setWindowTitle(LOCALIZATION("UpdateApplication"));
	setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	resize(SIZE_500_90);
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);
	ForbidResize();

	QLabel *LabelInformation = new QLabel(this);
	LabelInformation->setWordWrap(true);
	LabelInformation->setText(LOCALIZATION("LoadingUpdateText") + "\n\n" + LOCALIZATION("CurrentVersion") + ": " + ISVersion::GetInstance().GetVersion() + "\n" + LOCALIZATION("NewVersion") + ": " + version);
	GetMainLayout()->addWidget(LabelInformation);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	QLabel *Label = new QLabel(this);
	Label->setText(LOCALIZATION("Action") + ":");
	LayoutTitle->addWidget(Label);

	LabelStatus = new QLabel(this);
	LayoutTitle->addWidget(LabelStatus);

	LayoutTitle->addStretch();

	ProgressBar = new QProgressBar(this);
	GetMainLayout()->addWidget(ProgressBar);

	ButtonDialog = new ISButtonDialog(this, LOCALIZATION("StartDownloading"), LOCALIZATION("Annulment"));
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
	ISSystem::RepaintWidget(LabelStatus);
}
//-----------------------------------------------------------------------------
void ISUpdateDownloadForm::Start()
{
	ButtonDialog->SetApplyEnabled(false);
	
	bool Loaded = ISUpdate::GetInstance().LoadUpdate(FileID, FileName);
	if (Loaded)
	{
		ISMessageBox::ShowWarning(nullptr, LOCALIZATION("Message.Warning.NotCloseUpdating"));
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
