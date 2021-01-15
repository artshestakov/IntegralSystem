#include "ISStorageFilesSubSystem.h"
#include "ISLocalization.h"
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
//-----------------------------------------------------------------------------
ISStorageFilesSubSystem::ISStorageFilesSubSystem(QWidget *parent) : ISListBaseForm("_StorageFiles", parent)
{
	GetAction(ISNamespace::AT_Create)->setToolTip(LANG("StorageFiles.Create"));
	
	QAction *ActionSave = ISControls::CreateActionSave(this);
	connect(ActionSave, &QAction::triggered, this, &ISStorageFilesSubSystem::SaveFile);
	AddAction(ActionSave, true, true);
}
//-----------------------------------------------------------------------------
ISStorageFilesSubSystem::~ISStorageFilesSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISStorageFilesSubSystem::Create()
{
	QStringList StringList = ISFileDialog::GetOpenFileNames(this);
	if (!StringList.isEmpty())
	{
		ISProgressForm ProgressForm(StringList.size(), LANG("InsertingFiles"), this);
		ProgressForm.show();

		//Обходим все выбранные файлы
		bool Result = false;
		for (const QString &FilePath : StringList)
		{
			//Открываем файл
			QFile File(FilePath);
			if (!File.open(QIODevice::ReadOnly))
			{
				ISMessageBox::ShowWarning(&ProgressForm, LANG("Message.Error.NotOpenedFile").arg(FilePath));
				continue;
			}

			//Читаем и закрываем
			QByteArray ByteArray = File.readAll();
			File.close();

			int MaxSize = SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE);
			if (ByteArray.size() > MaxSize)
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.StorageFileBigSize").arg(FilePath).arg(MaxSize));
				continue;
			}
			ByteArray = ByteArray.toBase64();
			
			ISTcpQuery qAddFileStorage(API_FILE_STORAGE_ADD);
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
void ISStorageFilesSubSystem::CreateCopy()
{
	QString FileName = GetCurrentRecordValue("Name").toString();
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CreateCopyFile").arg(FileName)))
	{
		FileName = ISInputDialog::GetString(LANG("Named"), LANG("FileName") + ':', FileName);
		if (FileName.isEmpty())
		{
			return;
		}

		ISTcpQuery qFileStorageCopy(API_FILE_STORAGE_COPY);
		qFileStorageCopy.BindValue("ID", GetObjectID());
		qFileStorageCopy.BindValue("Name", FileName);

		ISGui::SetWaitGlobalCursor(true);
		bool Result = qFileStorageCopy.Execute();
		ISGui::SetWaitGlobalCursor(false);

		if (Result)
		{
			SetSelectObjectAfterUpdate(qFileStorageCopy.GetAnswer()["NewID"].toInt());
			Update();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.CreateCopyStorageFile"));
		}
	}
}
//-----------------------------------------------------------------------------
void ISStorageFilesSubSystem::SaveFile()
{
	QString Expansion = GetCurrentRecordValue("Expansion").toString(),
		Name = GetCurrentRecordValue("Name").toString();

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.SaveFile").arg(Name + '.' + Expansion)))
	{
		QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.File").arg(Expansion), Name);
		if (FilePath.isEmpty()) //Пользователь отказался от сохранения
		{
			return;
		}

		//Пытаемся открыть файл
		QFile File(FilePath);
		if (!File.open(QIODevice::WriteOnly))
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(FilePath), File.errorString());
			return;
		}

		//Запрашиваем файл
		ISTcpQuery qFileStorageGet(API_FILE_STORAGE_GET);
		qFileStorageGet.BindValue("ID", GetObjectID());

		ISGui::SetWaitGlobalCursor(true);
		bool Result = qFileStorageGet.Execute();
		ISGui::SetWaitGlobalCursor(false);

		if (Result) //Успешно - пишем файл на диск
		{
			File.write(QByteArray::fromBase64(qFileStorageGet.TakeAnswer()["Data"].toByteArray()));
			File.close();

			//Предлагаем открыть файл
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.File.SavedToPath").arg(FilePath)))
			{
				if (!ISGui::OpenFile(FilePath)) //Не удалось открыть файл
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(Name));
				}
			}
		}
		else //Ошибка
		{
			ISMessageBox::ShowCritical(this, qFileStorageGet.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
