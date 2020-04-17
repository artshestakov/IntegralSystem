#include "ISStorageFilesListForm.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISProgressForm.h"
#include "ISConstants.h"
#include "ISMessageBox.h"
#include "ISFileDialog.h"
#include "ISControls.h"
#include "ISInputDialog.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISDefinesCore.h"
#include "ISProgressForm.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
static QString QI_FILE = PREPARE_QUERY("INSERT INTO _storagefiles(sgfs_name, sgfs_expansion, sgfs_size, sgfs_private, sgfs_data) "
									   "VALUES (:Name, :Expansion, :Size, :Private, :Data) "
									   "RETURNING sgfs_id");
//-----------------------------------------------------------------------------
static QString QI_FILE_COPY = PREPARE_QUERY("INSERT INTO _storagefiles(sgfs_name, sgfs_expansion, sgfs_size, sgfs_private, sgfs_data) "
											"SELECT :Name, sgfs_expansion, sgfs_size, sgfs_private, sgfs_data "
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
	GetQueryModel()->SetClassFilter("NOT sgfs_private");

	ActionSetText(ISNamespace::AT_Create, LANG("StorageFiles.Create"));
	ActionSetToolTip(ISNamespace::AT_Create, LANG("StorageFiles.Create"));
	
	QAction *ActionSave = ISControls::CreateActionSave(this);
	connect(ActionSave, &QAction::triggered, this, &ISStorageFilesListForm::SaveFile);
	AddAction(ActionSave, true, true);

	RadioAllFiles = new QRadioButton(LANG("StorageFiles.AllFiles"), this);
	RadioAllFiles->setChecked(true);
	GetToolBar()->addWidget(RadioAllFiles);

	RadioMyFiles = new QRadioButton(LANG("StorageFiles.MyFiles"), this);
	GetToolBar()->addWidget(RadioMyFiles);

	RadioMyPrivateFiles = new QRadioButton(LANG("StorageFiles.MyPrivateFiles"), this);
	GetToolBar()->addWidget(RadioMyPrivateFiles);

	QButtonGroup *ButtonGroup = new QButtonGroup(this);
	ButtonGroup->addButton(RadioAllFiles);
	ButtonGroup->addButton(RadioMyFiles);
	ButtonGroup->addButton(RadioMyPrivateFiles);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &ISStorageFilesListForm::FilterChanged);
}
//-----------------------------------------------------------------------------
ISStorageFilesListForm::~ISStorageFilesListForm()
{

}
//-----------------------------------------------------------------------------
void ISStorageFilesListForm::Create()
{
	QStringList StringList = ISFileDialog::GetOpenFilesName(this);
	if (!StringList.isEmpty())
	{
		bool Private = ISMessageBox::ShowQuestion(this, StringList.size() > 1 ? LANG("Message.Question.AllStorageFileIsPrivate").arg(StringList.size()) : LANG("Message.Question.StorageFileIsPrivate")),
			IsUpdateList = false;
		ISVectorInt VectorInt;

		ISProgressForm ProgressForm(StringList.size(), LANG("InsertingFiles"), this);
		ProgressForm.show();

		for (const QString &FilePath : StringList)
		{
			ProgressForm.IncrementValue();

			QFileInfo FileInfo(FilePath);
			QFile File(FilePath);

			if (File.size() > (((1000 * 1024) * SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE))))
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.InsertingFileSizeVeryBig").arg(FileInfo.completeBaseName()).arg(SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE)));
				continue;
			}

			if (!File.open(QIODevice::ReadOnly))
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(FilePath));
				continue;
			}

			QByteArray ByteArray = File.readAll();
			File.close();

			
			ISQuery qInsert(QI_FILE);
			qInsert.BindValue(":Name", FileInfo.baseName());
			qInsert.BindValue(":Expansion", FileInfo.suffix());
			qInsert.BindValue(":Size", ISSystem::FileSizeFromString(FileInfo.size()));
			qInsert.BindValue(":Private", Private);
			qInsert.BindValue(":Data", ByteArray);

			ISGui::SetWaitGlobalCursor(true);
			bool Inserted = qInsert.ExecuteFirst();
			ISGui::SetWaitGlobalCursor(false);

			if (Inserted)
			{
				VectorInt.emplace_back(qInsert.ReadColumn("sgfs_id").toInt());
				IsUpdateList = true;
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.ErrorInsertFileToStorage").arg(FilePath), qInsert.GetErrorString());
			}

			if (ProgressForm.WasCanceled())
			{
				if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.StopInsertingToStorage")))
				{
					if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteInsetedFiles")))
					{
						QString SqlText = "DELETE FROM _storagefiles WHERE sgfs_id IN (";
						for (int ID : VectorInt)
						{
							SqlText += QString::number(ID) + ", ";
						}
						SqlText.chop(2);
						SqlText += ')';

						ISQuery qDelete(SqlText);
						if (!qDelete.Execute())
						{
							ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteInsetedFiles"));
						}
					}
					break;
				}
				else
				{
					ProgressForm.SetCanceled(false);
				}
			}
		}

		if (IsUpdateList)
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
void ISStorageFilesListForm::FilterChanged(QAbstractButton *AbstractButton)
{
	if (AbstractButton == RadioAllFiles)
	{
		GetQueryModel()->SetClassFilter("NOT sgfs_private");
	}
	else if (AbstractButton == RadioMyFiles)
	{
		GetQueryModel()->SetClassFilter("sgfs_owner = currentuserid() AND NOT sgfs_private");
	}
	else if (AbstractButton == RadioMyPrivateFiles)
	{
		GetQueryModel()->SetClassFilter("sgfs_owner = currentuserid() AND sgfs_private");
	}
	Update();
}
//-----------------------------------------------------------------------------
