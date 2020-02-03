#include "StdAfx.h"
#include "ISUpdate.h"
#include "ISAssert.h"
#include "ISDefines.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISVersion.h"
//-----------------------------------------------------------------------------
static QString QS_ACTUAL_INSTALL = PREPARE_QUERY("SELECT dsfs_id, dsfs_filename, dsfs_version, dsfs_version_4 FROM _distfiles WHERE NOT dsfs_isdeleted AND dsfs_actual");
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
ISUpdate::ISUpdate() : QObject()
{
	Loading = false;
	Stop = false;
	Value = 0;
}
//-----------------------------------------------------------------------------
ISUpdate::~ISUpdate()
{

}
//-----------------------------------------------------------------------------
ISUpdate& ISUpdate::GetInstance()
{
	static ISUpdate Update;
	return Update;
}
//-----------------------------------------------------------------------------
bool ISUpdate::GetLoading() const
{
	return Loading;
}
//-----------------------------------------------------------------------------
void ISUpdate::SetStop(bool stop)
{
	Stop = stop;
}
//-----------------------------------------------------------------------------
bool ISUpdate::CheckUpdate(int &FileID, QString &FileName, QString &Version)
{
	bool Result = false;
	ISQuery qSelect(QS_ACTUAL_INSTALL);
	if (qSelect.ExecuteFirst())
	{
		int Version_4 = qSelect.ReadColumn("dsfs_version_4").toInt();
		if (Version_4 > ISVersion::GetInstance().GetBuild())
		{
			FileID = qSelect.ReadColumn("dsfs_id").toInt();
			FileName = qSelect.ReadColumn("dsfs_filename").toString();
			Version = qSelect.ReadColumn("dsfs_version").toString();
			Result = true;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISUpdate::LoadUpdate(int FileID, const QString &FileName)
{
	Loading = true;
	emit Message(LANG("Preparation") + "...");

	ISQuery qSelectID(QS_DATA_ID);
	qSelectID.BindValue(":FileID", FileID);
	if (qSelectID.Execute())
	{
		emit ProgressMaximum(qSelectID.GetCountResultRows());

		QFile FileUpdate(PATH_TEMP_DIR + '/' + FileName + ".exe");
		if (FileUpdate.exists()) //Если файл такой обновления уже существует во временой папке - удалить его
		{
			emit Message(LANG("DeletingExistFileUpdate") + "...");
			IS_ASSERT(FileUpdate.remove(), FileUpdate.errorString());
		}

		IS_ASSERT(FileUpdate.open(QIODevice::WriteOnly | QIODevice::Append), FileUpdate.errorString());
		emit Message(LANG("Downloading") + "...");

		while (qSelectID.Next()) //Обход данных
		{
			if (Stop) //Если была нажата клавиша остановки
			{
				if (ISMessageBox::ShowQuestion(nullptr, LANG("Message.Question.UpdatingStop")))
				{
					FileUpdate.close();
					FileUpdate.remove();
					return false;
				}
				else
				{
					Stop = false;
				}
			}

			int DataID = qSelectID.ReadColumn("dfdt_id").toInt();

			ISQuery qSelectData(QS_DATA);
			qSelectData.BindValue(":DataID", DataID);
			if (qSelectData.ExecuteFirst())
			{
				QString Data = qSelectData.ReadColumn("dfdt_data").toString();
				FileUpdate.write(QByteArray::fromBase64(Data.toUtf8()));
			}

			Value++;
			emit ProgressValue(Value);
			ISGui::ProcessEvents();
		}

		FileUpdate.close();
		emit Message(LANG("DownloadingDone"));
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISUpdate::StartInstallUpdate(const QString &FileName)
{
	return QProcess::startDetached(PATH_APPLICATION_DIR + "/Update.cmd", QStringList() << FileName, PATH_APPLICATION_DIR);
}
//-----------------------------------------------------------------------------
