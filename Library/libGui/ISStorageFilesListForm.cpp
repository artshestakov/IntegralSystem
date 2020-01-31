#include "StdAfx.h"
#include "ISStorageFilesListForm.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISCore.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISProgressForm.h"
#include "ISConstants.h"
#include "ISMessageBox.h"
#include "ISMetaUser.h"
#include "ISFileDialog.h"
#include "ISControls.h"
#include "ISAttachFilesForm.h"
#include "ISAttachFileForm.h"
#include "ISInputDialog.h"
#include "ISBuffer.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QI_FILE_COPY = PREPARE_QUERY("INSERT INTO _storagefiles(sgfs_owneruser, sgfs_name, sgfs_expansion, sgfs_size) "
											"SELECT sgfs_owneruser, sgfs_name, sgfs_expansion, sgfs_size "
											"FROM _storagefiles "
											"WHERE sgfs_id = :ObjectID "
											"RETURNING sgfs_id");
//-----------------------------------------------------------------------------
static QString QI_FILE_COPY_DATA = PREPARE_QUERY("INSERT INTO _storagefilesdata(sgfd_storagefile, sgfd_data) "
												 "SELECT :StorageFile, sgfd_data "
												 "FROM _storagefilesdata "
												 "WHERE sgfd_storagefile = :StorageFile "
												 "ORDER BY sgfd_id");
//-----------------------------------------------------------------------------
static QString QS_FILE = PREPARE_QUERY("SELECT sgfd_data FROM _storagefilesdata WHERE sgfd_StorageFile = :StorageFile ORDER BY sgfd_id");
//-----------------------------------------------------------------------------
ISStorageFilesListForm::ISStorageFilesListForm(QWidget *parent) : ISListBaseForm("_StorageFiles", parent)
{
	GetQueryModel()->SetClassFilter("NOT sgfs_private");

	QAction *ActionCreateMore = new QAction(this);
	ActionCreateMore->setText(LANG("StorageFiles.AddMore"));
	ActionCreateMore->setToolTip(LANG("StorageFiles.AddMore"));
	ActionCreateMore->setIcon(BUFFER_ICONS("StorageFiles.AddMore"));
	connect(ActionCreateMore, &QAction::triggered, this, &ISStorageFilesListForm::AddMore);
	InsertAction(ActionCreateMore, GetAction(ISNamespace::AT_Create), false);

	ActionSetText(ISNamespace::AT_Create, LANG("StorageFiles.Create"));
	ActionSetToolTip(ISNamespace::AT_Create, LANG("StorageFiles.Create"));
	
	QAction *ActionSave = ISControls::CreateActionSave(this);
	connect(ActionSave, &QAction::triggered, this, &ISStorageFilesListForm::SaveFile);
	AddAction(ActionSave, true, true);
	
	QAction *ActionOpen = new QAction(this);
	ActionOpen->setText(LANG("StorageFiles.Open"));
	ActionOpen->setToolTip(LANG("StorageFiles.Open"));
	ActionOpen->setIcon(BUFFER_ICONS("File.Open"));
	connect(ActionOpen, &QAction::triggered, this, &ISStorageFilesListForm::OpenFile);
	AddAction(ActionSave, true, true);

	RadioAll = new QRadioButton(this);
	RadioAll->setText(LANG("StorageFiles.AllFiles"));
	RadioAll->setChecked(true);
	connect(RadioAll, &QRadioButton::clicked, this, &ISStorageFilesListForm::FilterChanged);
	GetToolBar()->addWidget(RadioAll);

	RadioPrivate = new QRadioButton(this);
	RadioPrivate->setText(LANG("StorageFiles.MyPrivateFiles"));
	connect(RadioPrivate, &QRadioButton::clicked, this, &ISStorageFilesListForm::FilterChanged);
	GetToolBar()->addWidget(RadioPrivate);
}
//-----------------------------------------------------------------------------
ISStorageFilesListForm::~ISStorageFilesListForm()
{

}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::AddMore()
{
	ISAttachFilesForm AttachmentFileForm;
	connect(&AttachmentFileForm, &ISAttachFilesForm::UpdateList, this, &ISStorageFilesListForm::Update);
	AttachmentFileForm.Exec();
}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::Create()
{
	ISAttachFileForm AttachFileForm;
	connect(&AttachFileForm, &ISAttachFileForm::UpdateList, this, &ISStorageFilesListForm::Update);
	AttachFileForm.Exec();
}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::CreateCopy()
{
	QString FileName = GetCurrentRecordValue("Name").toString();

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CreateCopyFile").arg(FileName)))
	{
		FileName = ISInputDialog::GetString(this, LANG("Named"), LANG("FileName"), FileName).toString();

		if (FileName.length())
		{
			ISQuery qInsertCopy(QI_FILE_COPY);
			qInsertCopy.BindValue(":ObjectID", GetObjectID());
			if (qInsertCopy.ExecuteFirst())
			{
				ISQuery qInsertData(QI_FILE_COPY_DATA);
				qInsertData.BindValue(":StorageFile", qInsertCopy.ReadColumn("sgfs_id").toInt());
				qInsertData.Execute();

				ISListBaseForm::Update();
			}
		}
		else
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.FileNameIsNull"));
		}
	}
}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::OpenFile()
{
	QFile File(APPLICATION_TEMP_PATH + "/" + ISSystem::GenerateUuid() + '.' + GetCurrentRecordValue("Expansion").toString());
	if (!File.open(QIODevice::WriteOnly))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(File.fileName()), File.errorString());
		return;
	}

	ISQuery qSelectFile(QS_FILE);
	qSelectFile.BindValue(":StorageFile", GetObjectID());
	if (qSelectFile.Execute())
	{
		while (qSelectFile.Next())
		{
			File.write(qSelectFile.ReadColumn("sgfd_data").toByteArray());
		}

		File.close();

		ISGui::OpenFile(File.fileName());
	}
}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::SaveFile()
{
	QString FileExpansion = GetCurrentRecordValue("Expansion").toString();
	QString FileName = GetCurrentRecordValue("Name").toString();

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.SaveFile").arg(FileName + '.' + FileExpansion)))
	{
		QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.File").arg(FileExpansion), FileName);
		if (!FilePath.length())
		{
			return;
		}

		QFile File(FilePath);
		if (!File.open(QIODevice::WriteOnly))
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(FilePath), File.errorString());
			return;
		}

		ISQuery qSelectFile(QS_FILE);
		qSelectFile.BindValue(":StorageFile", GetObjectID());
		if (qSelectFile.Execute())
		{
			while (qSelectFile.Next())
			{
				QByteArray ByteArray = qSelectFile.ReadColumn("sgfd_data").toByteArray();
				File.write(qUncompress(ByteArray));
			}

			File.close();

			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.File.SavedToPath").arg(FilePath)))
			{
				ISGui::OpenFile(FilePath);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::FilterChanged()
{
	QString ClassFilter;

	if (RadioAll->isChecked())
	{
		GetQueryModel()->SetClassFilter("NOT sgfs_private");
	}
	else if (RadioPrivate->isChecked())
	{
		GetQueryModel()->SetClassFilter("sgfs_owneruser = currentuserid() AND sgfs_private");
	}

	GetQueryModel()->SetClassFilter(ClassFilter);
	Update();
}
//-----------------------------------------------------------------------------
