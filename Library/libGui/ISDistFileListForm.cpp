#include "StdAfx.h"
#include "ISDistFileListForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISAssert.h"
#include "ISMessageBox.h"
#include "ISSystem.h"
#include "ISQuery.h"
#include "ISBuffer.h"
#include "ISFileDialog.h"
#include "ISProcessForm.h"
#include "ISControls.h"
#include "ISNotifySender.h"
#include "ISCore.h"
//-----------------------------------------------------------------------------
static QString QI_FILE = PREPARE_QUERY("INSERT INTO _distfiles(dsfs_filename, dsfs_size, dsfs_createddatefile, dsfs_lastmodifiedfile, dsfs_version, dsfs_actual, dsfs_version_1, dsfs_version_2, dsfs_version_3, dsfs_version_4) "
									   "VALUES(:FileName, :Size, :CreatedDateFile, :LastModifiedFile, :Version, :Actual, :Version_1, :Version_2, :Version_3, :Version_4) "
									   "RETURNING dsfs_id");
//-----------------------------------------------------------------------------
static QString QI_FILE_DATA = PREPARE_QUERY("INSERT INTO _distfilesdata(dfdt_file, dfdt_data) "
											"VALUES(:FileID, :Data)");
//-----------------------------------------------------------------------------
static QString QU_ACTUAL_DISABLE = PREPARE_QUERY("UPDATE _distfiles SET "
												 "dsfs_actual = false ");
//-----------------------------------------------------------------------------
static QString QU_ACTUAL = PREPARE_QUERY("UPDATE _distfiles SET "
										 "dsfs_actual = true "
										 "WHERE dsfs_id = :ObjectID");
//-----------------------------------------------------------------------------
static QString QS_FILE_DATA = PREPARE_QUERY("SELECT dfdt_data FROM _distfilesdata WHERE dfdt_file = :FileID ORDER BY dfdt_id");
//-----------------------------------------------------------------------------
static QString QS_FILE_VERSION_COUNT = PREPARE_QUERY("SELECT COUNT(*) FROM _distfiles WHERE dsfs_version_4 = :Version_4");
//-----------------------------------------------------------------------------
ISDistFileListForm::ISDistFileListForm(QWidget *parent) : ISListBaseForm("_DistFiles", parent)
{
	QWidget::setAcceptDrops(true);

	GetAction(ISNamespace::AT_Create)->setText(LANG("Addition"));

	QAction *ActionActual = new QAction(this);
	ActionActual->setText(LANG("ActualInstall"));
	ActionActual->setToolTip(LANG("ActualInstall"));
	ActionActual->setIcon(BUFFER_ICONS("Apply.Blue"));
	connect(ActionActual, &QAction::triggered, this, &ISDistFileListForm::Actual);
	AddAction(ActionActual);

	QAction *ActionSave = ISControls::CreateActionSave(this);
	connect(ActionSave, &QAction::triggered, this, &ISDistFileListForm::Save);
	AddAction(ActionSave);
}
//-----------------------------------------------------------------------------
ISDistFileListForm::~ISDistFileListForm()
{

}
//-----------------------------------------------------------------------------
void ISDistFileListForm::dragEnterEvent(QDragEnterEvent *e)
{
	ISListBaseForm::dragEnterEvent(e);
	
	const QMimeData *MimeData = e->mimeData();
	QList<QUrl> Urls = MimeData->urls();
	if (Urls.count() == 1)
	{
		QString FilePath = Urls.at(0).toLocalFile();
		QFileInfo FileInfo(FilePath);
		if (FileInfo.suffix() == "exe")
		{
			e->accept();
		}
	}
}
//-----------------------------------------------------------------------------
void ISDistFileListForm::dropEvent(QDropEvent *e)
{
	ISListBaseForm::dropEvent(e);
	InsertFile(e->mimeData()->urls().at(0).toLocalFile());
}
//-----------------------------------------------------------------------------
void ISDistFileListForm::Create()
{
	QString FilePath = ISFileDialog::GetOpenFileName(this, QString(), LANG("File.Filter.Exe"));
	if (FilePath.length())
	{
		InsertFile(FilePath);
	}
}
//-----------------------------------------------------------------------------
bool ISDistFileListForm::DeleteCascade()
{
	bool Result = ISListBaseForm::DeleteCascade();
	if (Result)
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.VacuumDistTable")))
		{
			ISProcessForm ProcessForm;
			ProcessForm.show();
			ProcessForm.SetText(LANG("VacuumTableDist"));

			ISQuery qVacuum;
			qVacuum.Execute("VACUUM _distfiles");
			qVacuum.Execute("VACUUM _distfilesdata");
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISDistFileListForm::InsertFile(const QString &FilePath)
{
	ISProgressForm ProgressForm(0, 0, this);
	ProgressForm.show();

	ProgressForm.SetText(LANG("OpeningFile") + "...");

	QFile FileUpdate(FilePath);
	if (!FileUpdate.open(QIODevice::ReadOnly))
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ErrorOpenedFile"), FileUpdate.errorString());
		return;
	}

	QString FileName = ISSystem::GetCompleteBaseFileName(FilePath);
	QString FileSize = ISSystem::FileSizeFromString(ISSystem::GetFileSize(FilePath));
	QDateTime CreatedDate = ISSystem::GetCreatedDateFile(FilePath);
	QDateTime LastModified = ISSystem::GetLastModifiedFile(FilePath);
	QString FileVersion = ISCore::GetVersionInFileName(FileName);

	QStringList Version = FileVersion.split('.');
	IS_ASSERT(Version.count() == 4, "Invalid version file");
	int Version_1 = Version[0].toInt();
	int Version_2 = Version[1].toInt();
	int Version_3 = Version[2].toInt();
	int Version_4 = Version[3].toInt();

	ISQuery qSelect(QS_FILE_VERSION_COUNT);
	qSelect.BindValue(":Version_4", Version_4);
	qSelect.ExecuteFirst();
	int Count = qSelect.ReadColumn("count").toInt();
	if (Count)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.VersionAlreadyExist").arg(FileVersion));
		return;
	}
	
	ProgressForm.SetText(LANG("AddingFile") + "...");

	ISQuery qActualDisable(QU_ACTUAL_DISABLE);
	if (qActualDisable.Execute())
	{
		ISQuery qInsertFile(QI_FILE);
		qInsertFile.BindValue(":FileName", FileName);
		qInsertFile.BindValue(":Size", FileSize);
		qInsertFile.BindValue(":CreatedDateFile", CreatedDate);
		qInsertFile.BindValue(":LastModifiedFile", LastModified);
		qInsertFile.BindValue(":Version", FileVersion);
		qInsertFile.BindValue(":Actual", true);
		qInsertFile.BindValue(":Version_1", Version_1);
		qInsertFile.BindValue(":Version_2", Version_2);
		qInsertFile.BindValue(":Version_3", Version_3);
		qInsertFile.BindValue(":Version_4", Version_4);
		if (qInsertFile.ExecuteFirst())
		{
			int FileID = qInsertFile.ReadColumn("dsfs_id").toInt();

			ProgressForm.SetText(LANG("ReadFile") + SYMBOL_SPACE + FileName + " ...");

			QString Base64 = FileUpdate.readAll().toBase64();
			IS_ASSERT(Base64.length(), QString("File \"%1\" is null").arg(FilePath));
			FileUpdate.close();

			int Part = 0;
			int CountBlocks = 0;

			for (int i = 0; i < Base64.count(); i += 1000000)
			{
				CountBlocks++;
			}

			ProgressForm.setMaximum(CountBlocks);

			for (int i = 0; i < Base64.count(); i += 1000000)
			{
				QString Temp = Base64.mid(i, 1000000);

				ProgressForm.setValue(Part);
				ProgressForm.SetText(LANG("AddingDataFile").arg(Part).arg(CountBlocks) + "...");

				if (!InsertData(FileID, Temp))
				{
					Temp.clear();
					ISMessageBox::ShowCritical(this, LANG("Message.Error.InsertFileDataInstaller").arg(FilePath));
					break;
				}

				Part++;
			}

			ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_UPDATE_AVAILABLE, QVariant(), QString(), false);
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
bool ISDistFileListForm::InsertData(int FileID, const QString &Data)
{
	ISQuery qInsertData(QI_FILE_DATA);
	qInsertData.BindValue(":FileID", FileID);
	qInsertData.BindValue(":Data", Data);
	return qInsertData.Execute();
}
//-----------------------------------------------------------------------------
void ISDistFileListForm::Actual()
{
	if (GetCurrentRecordValue("Actual").toBool())
	{
		ISMessageBox::ShowInformation(this, LANG("Message.Information.SelectedUpdateAlreadyActual"));
	}
	else
	{
		ISQuery qActualDisable(QU_ACTUAL_DISABLE);
		if (qActualDisable.Execute())
		{
			ISQuery qActual(QU_ACTUAL);
			qActual.BindValue(":ObjectID", GetObjectID());
			if (qActual.Execute())
			{
				Update();
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISDistFileListForm::Save()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.SaveDist")))
	{
		QString FileName = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.Exe"), GetCurrentRecordValueDB("FileName").toString());
		if (FileName.length())
		{
			QFile FileDist(FileName);
			if (FileDist.exists())
			{
				FileDist.remove();
			}

			ISProcessForm ProcessForm;
			ProcessForm.show();
			ProcessForm.SetText(LANG("SavingDistFile"));

			ISQuery qSelectData(QS_FILE_DATA);
			qSelectData.BindValue(":FileID", GetObjectID());
			if (qSelectData.Execute())
			{
				if (FileDist.open(QIODevice::WriteOnly | QIODevice::Append))
				{
					while (qSelectData.Next())
					{
						QString Data = qSelectData.ReadColumn("dfdt_data").toString();
						FileDist.write(QByteArray::fromBase64(Data.toUtf8()));
					}

					ProcessForm.close();
					FileDist.close();
					ISMessageBox::ShowInformation(this, LANG("Message.Information.SaveDistFile"));
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
