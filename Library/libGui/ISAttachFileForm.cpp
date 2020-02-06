#include "ISAttachFileForm.h"
#include "ISLocalization.h"
#include "ISStorageFileLoader.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISAttachFileForm::ISAttachFileForm(const QString &FilePath, QWidget *parent) : ISAttachFileBaseForm(parent)
{
	setWindowTitle(LANG("AddingFiles"));
	setMinimumWidth(500);
	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_10_PX);
	ForbidResize();

	connect(this, &ISAttachFileForm::UpdateList, this, &ISAttachFileForm::close);

	QHBoxLayout *LayoutFile = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutFile);

	QLabel *LabelFile = new QLabel(this);
	LabelFile->setText(LANG("File") + ':');
	LayoutFile->addWidget(LabelFile);

	PathFileEdit = new ISPathEditFile(this);
	connect(PathFileEdit, &ISPathEditFile::ValueChange, this, &ISAttachFileForm::EditFileChanged);
	LayoutFile->addWidget(PathFileEdit);

	QHBoxLayout *LayoutNote = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutNote);

	QLabel *LabelNote = new QLabel(this);
	LabelNote->setText(LANG("Note") + ':');
	LayoutNote->addWidget(LabelNote);

	NoteEdit = new ISLineEdit(this);
	NoteEdit->SetPlaceholderText(LANG("InputNoteFileStorage"));
	LayoutNote->addWidget(NoteEdit);

	ProgressBar = new QProgressBar(this);
	ProgressBar->setValue(0);
	GetMainLayout()->addWidget(ProgressBar);

	ButtonDialog = new ISButtonDialog(this, LANG("Download"));
	ButtonDialog->SetApplyEnabled(false);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISAttachFileForm::StartDownload);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISAttachFileForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISAttachFileForm::~ISAttachFileForm()
{

}
//-----------------------------------------------------------------------------
void ISAttachFileForm::SetEnabledPath(bool Enabled)
{
	PathFileEdit->setEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISAttachFileForm::SetFilePath(const QString &FilePath)
{
	PathFileEdit->SetValue(FilePath);
}
//-----------------------------------------------------------------------------
void ISAttachFileForm::StartDownload()
{
	ISStorageFileLoader StorageFileLoader(PathFileEdit->GetValue().toString(), NoteEdit->GetValue().toString(), this);
	connect(&StorageFileLoader, &ISStorageFileLoader::ChangeProgressFileMaximum, ProgressBar, &QProgressBar::setMaximum);
	connect(&StorageFileLoader, &ISStorageFileLoader::LoadingFile, [=]	{ ProgressBar->setValue(ProgressBar->value() + 1); });
	if (StorageFileLoader.Load())
	{
		PathFileEdit->Clear();
		ProgressBar->setValue(0);
		ButtonDialog->SetApplyEnabled(false);

		emit Loaded(StorageFileLoader.GetStorageFileID());
		emit UpdateList();
	}
}
//-----------------------------------------------------------------------------
void ISAttachFileForm::EditFileChanged(const QVariant &value)
{
	if (value.isValid())
	{
		ButtonDialog->SetApplyEnabled(true);
	}
	else
	{
		ButtonDialog->SetApplyEnabled(false);
	}
}
//-----------------------------------------------------------------------------
