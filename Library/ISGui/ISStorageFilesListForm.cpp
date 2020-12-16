#include "ISStorageFilesListForm.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISTcpQuery.h"
#include "ISProgressForm.h"
#include "ISConstants.h"
#include "ISMessageBox.h"
#include "ISFileDialog.h"
#include "ISControls.h"
#include "ISInputDialog.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISProgressForm.h"
#include "ISSettingsDatabase.h"
#include "ISMetaUser.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
static QString QI_FILE = PREPARE_QUERY("INSERT INTO _storagefiles(sgfs_owner, sgfs_name, sgfs_expansion, sgfs_size, sgfs_data) "
									   "VALUES(:Owner, :Name, :Expansion, :Size, :Data) "
									   "RETURNING sgfs_id");
//-----------------------------------------------------------------------------
static QString QI_FILE_COPY = PREPARE_QUERY("INSERT INTO _storagefiles(sgfs_name, sgfs_expansion, sgfs_size, sgfs_data) "
											"SELECT :Name, sgfs_expansion, sgfs_size, sgfs_data "
											"FROM _storagefiles "
											"WHERE sgfs_id = :ObjectID "
											"RETURNING sgfs_id");
//-----------------------------------------------------------------------------
static QString QS_FILE = PREPARE_QUERY("SELECT sgfs_data "
									   "FROM _storagefiles "
									   "WHERE sgfs_id = :ObjectID");
//-----------------------------------------------------------------------------
ISStorageFilesListForm::ISStorageFilesListForm(QWidget *parent) : ISListBaseForm("_StorageFiles", parent)
{
	GetAction(ISNamespace::AT_Create)->setToolTip(LANG("StorageFiles.Create"));
	
	QAction *ActionSave = ISControls::CreateActionSave(this);
	connect(ActionSave, &QAction::triggered, this, &ISStorageFilesListForm::SaveFile);
	AddAction(ActionSave, true, true);
}
//-----------------------------------------------------------------------------
ISStorageFilesListForm::~ISStorageFilesListForm()
{

}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::Create()
{
	QStringList StringList = ISFileDialog::GetOpenFileNames(this);
	if (!StringList.isEmpty())
	{
		ISProgressForm ProgressForm(StringList.size(), LANG("InsertingFiles"), this);
		ProgressForm.show();

		//������� ��� ��������� �����
		bool Result = false;
		for (const QString &FilePath : StringList)
		{
			//��������� ����
			QFile File(FilePath);
			if (!File.open(QIODevice::ReadOnly))
			{
				ISMessageBox::ShowWarning(&ProgressForm, LANG("Message.Error.NotOpenedFile").arg(FilePath));
				continue;
			}

			//������ � ���������
			QByteArray ByteArray = File.readAll().toBase64();
			File.close();
			
			ISTcpQuery qAddFileStorage(API_ADD_FILE_STORAGE);
			qAddFileStorage.BindValue("FileName", QFileInfo(FilePath).fileName());
			qAddFileStorage.BindValue("Data", ByteArray);

			ISGui::SetWaitGlobalCursor(true);
			Result = qAddFileStorage.Execute();
			ISGui::SetWaitGlobalCursor(false);

			if (!Result)
			{
				ISMessageBox::ShowCritical(&ProgressForm, qAddFileStorage.GetErrorString());
			}

			if (ProgressForm.WasCanceled())
			{
				if (ISMessageBox::ShowQuestion(&ProgressForm, LANG("Message.Question.StopInsertingToStorage")))
				{
					break;
				}
				else
				{
					ProgressForm.SetCanceled(false);
				}
			}
			ProgressForm.IncrementValue();
		}

		if (Result)
		{
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::CreateCopy()
{
	QString FileName = GetCurrentRecordValue("Name").toString();
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CreateCopyFile").arg(FileName)))
	{
		FileName = ISInputDialog::GetString(LANG("Named"), LANG("FileName") + ':', FileName);
		if (!FileName.isEmpty())
		{
			ISQuery qInsertCopy(QI_FILE_COPY);
			qInsertCopy.BindValue(":Name", FileName);
			qInsertCopy.BindValue(":ObjectID", GetObjectID());

			ISGui::SetWaitGlobalCursor(true);
			bool Inserted = qInsertCopy.ExecuteFirst();
			ISGui::SetWaitGlobalCursor(false);
			
			if (Inserted)
			{
				SetSelectObjectAfterUpdate(qInsertCopy.ReadColumn("sgfs_id").toInt());
				Update();
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.CreateCopyStorageFile"));
			}
		}
		else
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.FileNameIsNull"));
		}
	}
}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::SaveFile()
{
	QString FileExpansion = GetCurrentRecordValue("Expansion").toString();
	QString FileName = GetCurrentRecordValue("Name").toString();
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.SaveFile").arg(FileName)))
	{
		QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.File").arg(FileExpansion), FileName);
		if (FilePath.isEmpty())
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
		qSelectFile.BindValue(":ObjectID", GetObjectID());
		if (qSelectFile.ExecuteFirst())
		{
			File.write(qSelectFile.ReadColumn("sgfs_data").toByteArray());
			File.close();
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.File.SavedToPath").arg(FilePath)))
			{
				if (!ISGui::OpenFile(FilePath))
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(FileName));
				}
			}
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.SelectStorageFileData"));
		}
	}
}
//-----------------------------------------------------------------------------
