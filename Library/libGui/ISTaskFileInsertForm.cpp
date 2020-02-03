#include "StdAfx.h"
#include "ISTaskFileInsertForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QI_FILE = PREPARE_QUERY("INSERT INTO _taskfile(tfls_task, tfls_user, tfls_filename, tfls_size, tfls_icon, tfls_note) "
									   "VALUES(:TaskID, currentuserid(), :FileName, :Size, :Icon, :Note) "
									   "RETURNING tfls_id");
//-----------------------------------------------------------------------------
static QString QI_FILE_DATA = PREPARE_QUERY("INSERT INTO _taskfiledata(tfdt_taskfile, tfdt_data) "
											"VALUES(:TaskFileID, :Data)");
//-----------------------------------------------------------------------------
ISTaskFileInsertForm::ISTaskFileInsertForm(int task_id, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	TaskID = task_id;

	setWindowTitle(LANG("Task.AttachingFile"));
	setMinimumWidth(500);
	GetMainLayout()->setContentsMargins(MARGINS_LAYOUT_5_PX);
	ForbidResize();

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	PathEditFile = new ISPathEditFile(this);
	connect(PathEditFile, &ISPathEditFile::DataChanged, this, &ISTaskFileInsertForm::PathEditfileChanged);
	FormLayout->addRow(LANG("File") + ':', PathEditFile);

	EditNote = new ISLineEdit(this);
	EditNote->SetPlaceholderText(LANG("Task.FileNote"));
	FormLayout->addRow(LANG("Note") + ':', EditNote);

	ProgressBar = new QProgressBar(this);
	ProgressBar->setValue(0);
	GetMainLayout()->addWidget(ProgressBar);

	ButtonDialog = new ISButtonDialog(this, LANG("Task.AttachFile"));
	ButtonDialog->SetApplyEnabled(false);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISTaskFileInsertForm::Insert);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISTaskFileInsertForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISTaskFileInsertForm::~ISTaskFileInsertForm()
{

}
//-----------------------------------------------------------------------------
QString ISTaskFileInsertForm::GetFileName() const
{
	return ISSystem::GetFileName(PathEditFile->GetValue().toString());
}
//-----------------------------------------------------------------------------
void ISTaskFileInsertForm::EnterClicked()
{
	if (PathEditFile->GetValue().toString().length())
	{
		Insert();
	}
}
//-----------------------------------------------------------------------------
void ISTaskFileInsertForm::PathEditfileChanged()
{
	if (PathEditFile->GetValue().toString().length())
	{
		ButtonDialog->SetApplyEnabled(true);
	}
	else
	{
		ButtonDialog->SetApplyEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISTaskFileInsertForm::Insert()
{
	QString FilePath = PathEditFile->GetValue().toString();
	QFile File(FilePath);
	if (File.open(QIODevice::ReadOnly)) //Если файл открылся
	{
		ISQuery qInsertFile(QI_FILE);
		qInsertFile.BindValue(":TaskID", TaskID);
		qInsertFile.BindValue(":FileName", ISSystem::GetFileName(FilePath));
		qInsertFile.BindValue(":Size", ISSystem::FileSizeFromString(ISSystem::GetFileSize(FilePath)));
		qInsertFile.BindValue(":Icon", ISGui::IconToByteArray(ISGui::GetIconFile(FilePath)));

		if (EditNote->GetValue().toString().length())
		{
			qInsertFile.BindValue(":Note", EditNote->GetValue());
		}
		else
		{
			qInsertFile.BindValue(":Note", QVariant());
		}

		if (qInsertFile.ExecuteFirst())
		{
			int TaskFileID = qInsertFile.ReadColumn("tfls_id").toInt();

			ProgressBar->setMaximum(File.size() / 524288); //Установка максимального значения

			while (!File.atEnd()) //Обход файла
			{
				QByteArray ByteArray = File.read(524288); //512kb

				ISQuery qInsertFileData(QI_FILE_DATA);
				qInsertFileData.BindValue(":TaskFileID", TaskFileID);
				qInsertFileData.BindValue(":Data", ByteArray);
				if (qInsertFileData.Execute())
				{
					ProgressBar->setValue(ProgressBar->value() + 1);
				}
			}

			SetResult(true);
			close();
		}
	}
	else //Файл не открылся
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(FilePath), File.errorString());
	}
}
//-----------------------------------------------------------------------------
