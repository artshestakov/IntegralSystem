#include "ISCoreScheduler.h"
#include "ISTypes.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISConfig.h"
#include "ISLogger.h"
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
		for (PMetaClassField *MetaField : MetaTable->Fields) //Обход всех пользовательских полей текущей сущности
		{
			if (MetaField->Type == ISNamespace::FT_File) //Если тип данных текущего поля является хранением файлов
			{
				QString SqlQuery = "SELECT " + MetaTable->Alias.toLower() + '_' + MetaField->Name.toLower() + " \n";
				SqlQuery += "FROM " + MetaTable->Name.toLower() + " \n";
				SqlQuery += "WHERE " + MetaTable->Alias.toLower() + '_' + MetaField->Name.toLower() + " IS NOT NULL";
				ISQuery qSelect(SqlQuery);
				qSelect.SetShowLongQuery(false);
				if (qSelect.Execute())
				{
					while (qSelect.Next())
					{
						VectorInt.removeAll(qSelect.ReadColumn(MetaTable->Alias + '_' + MetaField->Name).toInt());
					}
				}
			}
		}
	}

	int Index = 0;
	for (int FileID : VectorInt) //Обход и удаление не привязанных файлов
	{
		++Index;
		ISLOGGER_INFO("Deleting file " + QString::number(Index) + " of " + QString::number(VectorInt.count()));

		ISQuery qDeleteFile(QD_FILE);
		qDeleteFile.SetShowLongQuery(false);
		qDeleteFile.BindValue(":FileID", FileID);
		qDeleteFile.Execute();
	}
}
//-----------------------------------------------------------------------------
