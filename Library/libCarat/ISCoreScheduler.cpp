#include "ISCoreScheduler.h"
#include "ISTypes.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISLicense.h"
#include "ISDebug.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static QString QS_FILE = PREPARE_QUERY("SELECT file_id "
									   "FROM _file "
									   "ORDER BY file_id");
//-----------------------------------------------------------------------------
static QString QD_FILE = PREPARE_QUERY("DELETE FROM _file "
									   "WHERE file_id = :FileID");
//-----------------------------------------------------------------------------
ISCoreScheduler::ISCoreScheduler(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreScheduler::~ISCoreScheduler()
{

}
//-----------------------------------------------------------------------------
bool ISCoreScheduler::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		ISMetaData::GetInstanse().Initialize(ISLicense::GetInstance().GetName(), false, false);

		Timer = new QTimer(this);
		Timer->setInterval(1000 * 60 * 60); //1 час
		connect(Timer, &QTimer::timeout, this, &ISCoreScheduler::Timeout);
		Timeout();
		Started();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreScheduler::Timeout()
{
	Timer->stop();
	ClearFiles();
	Timer->start();
}
//-----------------------------------------------------------------------------
void ISCoreScheduler::ClearFiles()
{
	QVectorInt VectorInt;

	//Запрос всех идентификаторов файлов
	ISQuery qSelectFile(QS_FILE);
	qSelectFile.SetShowLongQuery(false);
	if (qSelectFile.Execute())
	{
		while (qSelectFile.Next())
		{
			VectorInt.append(qSelectFile.ReadColumn("file_id").toInt());
		}
	}
	
	for (PMetaClassTable *MetaTable : ISMetaData::GetInstanse().GetTables()) //Обход всех сущностей
	{
		for (PMetaClassField *MetaField : MetaTable->GetFields()) //Обход всех пользовательских полей текущей сущности
		{
			if (MetaField->GetType() == ISNamespace::FT_File) //Если тип данных текущего поля является хранением файлов
			{
				QString SqlQuery = "SELECT " + MetaTable->GetAlias().toLower() + '_' + MetaField->GetName().toLower() + " \n";
				SqlQuery += "FROM " + MetaTable->GetName().toLower() + " \n";
				SqlQuery += "WHERE " + MetaTable->GetAlias().toLower() + '_' + MetaField->GetName().toLower() + " IS NOT NULL";
				ISQuery qSelect(SqlQuery);
				qSelect.SetShowLongQuery(false);
				if (qSelect.Execute())
				{
					while (qSelect.Next())
					{
						int FileID = qSelect.ReadColumn(MetaTable->GetAlias() + '_' + MetaField->GetName()).toInt();
						VectorInt.removeAll(FileID);
					}
				}
			}
		}
	}

	int Index = 0;
	for (int FileID : VectorInt) //Обход и удаление не привязанных файлов
	{
		Index++;
		ISDebug::ShowInfoString(LANG("Core.Scheduler.Delete").arg(Index).arg(VectorInt.count()));

		ISQuery qDeleteFile(QD_FILE);
		qDeleteFile.SetShowLongQuery(false);
		qDeleteFile.BindValue(":FileID", FileID);
		qDeleteFile.Execute();
	}
}
//-----------------------------------------------------------------------------
