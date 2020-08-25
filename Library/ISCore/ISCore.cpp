#include "ISCore.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISQueryPool.h"
#include "ISLogger.h"
#include "ISProtocol.h"
#include "ISSystem.h"
#include "ISSettings.h"
#include "ISMetaUser.h"
#include "ISSettingsDatabase.h"
#include "ISCrashDumper.h"
#include "ISDefinesCore.h"
#include "ISDatabase.h"
#include "ISTcpConnector.h"
#include "ISMetaDataHelper.h"
//-----------------------------------------------------------------------------
static QString QI_CALENDAR = PREPARE_QUERY("INSERT INTO _calendar(cldr_date, cldr_timealert, cldr_name, cldr_text, cldr_tablename, cldr_objectid) "
                                           "VALUES(:Date, :TimeAlert, :Name, :Text, :TableName, :ObjectID) "
										   "RETURNING cldr_id");
//-----------------------------------------------------------------------------
static QString QU_CALENDAR_CLOSE = PREPARE_QUERY("UPDATE _calendar SET cldr_closed = true WHERE cldr_id = :CalendarID");
//-----------------------------------------------------------------------------
static QString QS_TASK_COUNT = PREPARE_QUERY("SELECT COUNT(*) FROM _task WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
static QString QU_DELETE_OBJECT = PREPARE_QUERY2("UPDATE %1 SET "
												 "%2_isdeleted = :IsDeleted, "
												 "%2_deletiondate = now(), "
												 "%2_deletionuser = CURRENT_USER "
												 "WHERE %2_id = :ObjectID");
//-----------------------------------------------------------------------------
static QString QU_RECOVERY_OBJECT = PREPARE_QUERY2("UPDATE %1 SET "
												   "%2_isdeleted = :IsDeleted "
												   "WHERE %2_id = :ObjectID");
//-----------------------------------------------------------------------------
static QString QU_DELETE_OBJECTS = PREPARE_QUERY2("UPDATE %1 SET "
												  "%2_isdeleted = :IsDeleted, "
												  "%2_deletiondate = now(), "
												  "%2_deletionuser = CURRENT_USER "
												  "WHERE %2_id IN(%3)");
//-----------------------------------------------------------------------------
static QString QU_RECOVERY_OBJECTS = PREPARE_QUERY2("UPDATE %1 SET "
													"%2_isdeleted = :IsDeleted "
													"WHERE %2_id IN(%3)");
//-----------------------------------------------------------------------------
bool ISCore::Startup(bool IsGui, const QString &ConfigTemplateName, QString &ErrorString)
{
	ISDefines::Core::Init(IsGui);

	bool Result = ISSystem::CreateDir(ISDefines::Core::PATH_TEMP_DIR, ErrorString); //Создание папки для временных файлов
	if (!Result)
	{
		return Result;
	}

	Result = ISSystem::CreateDir(ISDefines::Core::PATH_CRASH_DIR, ErrorString); //Создание папки для crash-файлов
	if (!Result)
	{
		return Result;
	}

	ISCrashDumper::Init();

	Result = ISLogger::Instance().Initialize();
	if (!Result)
	{
		ISLOGGER_L(ISLogger::Instance().GetErrorString());
		return Result;
	}

    //Установим кодировку для консольного приложения под Windows
#ifdef WIN32
	if (!IsGui)
	{
		Result = SetConsoleOutputCP(65001) == TRUE ? true : false;
	}
#endif

	if (!Result)
	{
		ISLOGGER_W("Error changed console encoding");
	}

	Result = ISConfig::Instance().Initialize(ConfigTemplateName);
	if (!Result)
	{
		ErrorString = ISConfig::Instance().GetErrorString();
		return Result;
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCore::ExitApplication()
{
	ISQueryPool::Instance().Shutdown();
	ISDatabase::Instance().DisconnectAll();
	ISTcpConnector::Instance().Disconnect();
	ISLogger::Instance().Shutdown();
	QCoreApplication::quit();
}
//-----------------------------------------------------------------------------
QString ISCore::GetObjectName(const QString &TableName, int ObjectID)
{
	return GetObjectName(ISMetaData::Instance().GetMetaTable(TableName), ObjectID);
}
//-----------------------------------------------------------------------------
QString ISCore::GetObjectName(PMetaTable *MetaTable, int ObjectID)
{
	if (MetaTable->TitleName.isEmpty()) //Если TitleName у мета-таблицы не заполнен - возвращаем идентификатор объекта
	{
		return ObjectID;
	}
	
	QString ObjectName;
	QStringList StringList = MetaTable->TitleName.split(';');
	QString QueryText = "SELECT ";

	if (StringList.count() > 1) //Если имя объекта строится из нескольких полей
	{
		QueryText += "concat(";
		for (const QString &FieldName : StringList) //Обход полей
		{
			PMetaForeign *MetaForeign = MetaTable->GetField(FieldName)->Foreign;
			QueryText += MetaForeign ?
				("(" + ISMetaDataHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, FieldName) + "), ' ', ") :
				(MetaTable->Alias + '_' + FieldName + ", ' ', ");
		}
		QueryText.chop(7);
		QueryText += ") \n";
	}
	else //Если указано только одно поле
	{
		PMetaForeign *MetaForeign = MetaTable->GetField(MetaTable->TitleName)->Foreign;
		QueryText += MetaForeign ?
			("(" + ISMetaDataHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, MetaTable->TitleName) + ") \n") :
			(MetaTable->Alias + '_' + MetaTable->TitleName + " \n");
	}

	QueryText += "FROM " + MetaTable->Name + " \n";
	QueryText += "WHERE " + MetaTable->Alias + "_id = :ObjectID";

	ISQuery qSelectName(QueryText);
	qSelectName.BindValue(":ObjectID", ObjectID);
	if (qSelectName.ExecuteFirst())
	{
		QVariant Value = qSelectName.ReadColumn(0);
		switch (Value.type())
		{
		case QVariant::Date: ObjectName = Value.toDate().toString(SETTING_STRING(CONST_UID_SETTING_OTHER_DATE_FORMAT)); break;
		case QVariant::Time: ObjectName = Value.toTime().toString(SETTING_STRING(CONST_UID_SETTING_OTHER_TIME_FORMAT)); break;
		case QVariant::DateTime: ObjectName = Value.toDateTime().toString(SETTING_STRING(CONST_UID_SETTING_OTHER_DATE_FORMAT) + SETTING_STRING(CONST_UID_SETTING_OTHER_TIME_FORMAT)); break;
		default: ObjectName = qSelectName.ReadColumn(0).toString();
		}
	}

	//Удаляем возможные пробелы в конце имени объекта
	ISSystem::RemoveLastSymbolLoop(ObjectName, SYMBOL_SPACE);
	return ObjectName;
}
//-----------------------------------------------------------------------------
int ISCore::CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName, int ObjectID)
{
	int CalendarID = 0;

	ISQuery qInsertCalendar(QI_CALENDAR);
	qInsertCalendar.BindValue(":Date", DateTime.date());
	qInsertCalendar.BindValue(":TimeAlert", DateTime.time());
	qInsertCalendar.BindValue(":Name", Name);
	qInsertCalendar.BindValue(":Text", Text);

	if (TableName.isEmpty())
	{
		qInsertCalendar.BindValue(":TableName", QVariant());
		qInsertCalendar.BindValue(":ObjectID", QVariant());
	}
	else
	{
		IS_ASSERT(ObjectID, "Invalid object id");
		qInsertCalendar.BindValue(":TableName", TableName);
		qInsertCalendar.BindValue(":ObjectID", ObjectID);
	}

	if (qInsertCalendar.ExecuteFirst())
	{
		CalendarID = qInsertCalendar.ReadColumn("cldr_id").toInt();
	}

	return CalendarID;
}
//-----------------------------------------------------------------------------
bool ISCore::CalendarCloseEvent(int CalendarID)
{
	ISQuery qCloseEvent(QU_CALENDAR_CLOSE);
	qCloseEvent.BindValue(":CalendarID", CalendarID);
	return qCloseEvent.Execute();
}
//-----------------------------------------------------------------------------
bool ISCore::TaskCheckExist(int TaskID)
{
	ISQuery qSelect(QS_TASK_COUNT);
	qSelect.BindValue(":TaskID", TaskID);
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		Result = qSelect.ReadColumn("count").toInt() > 0;
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISCore::SetIsDeletedObject(bool IsDeleted, PMetaTable *MetaTable, int ObjectID, QString &ErrorString)
{
	ISQuery qQuery((IsDeleted ? QU_DELETE_OBJECT : QU_RECOVERY_OBJECT).arg(MetaTable->Name).arg(MetaTable->Alias));
	qQuery.BindValue(":IsDeleted", IsDeleted);
	qQuery.BindValue(":ObjectID", ObjectID);
	bool Result = qQuery.Execute();
	if (Result)
	{
		ISProtocol::DeleteObject(MetaTable->Name, MetaTable->LocalListName, ObjectID);
	}
	else
	{
		ErrorString = qQuery.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISCore::SetIsDeletedObjects(bool IsDeleted, PMetaTable *MetaTable, const ISVectorInt &Objects, QString &ErrorString)
{
	QStringList StringList;
	for (int ObjectID : Objects)
	{
		StringList.push_back(QString::number(ObjectID));
	}

	ISQuery qQuery((IsDeleted ? QU_DELETE_OBJECTS : QU_RECOVERY_OBJECTS).arg(MetaTable->Name).arg(MetaTable->Alias).arg(StringList.join(',')));
	qQuery.BindValue(":IsDeleted", IsDeleted);
	bool Result = qQuery.Execute();
	if (Result)
	{
		for (int ObjectID : Objects)
		{
			ISProtocol::DeleteObject(MetaTable->Name, MetaTable->LocalListName, ObjectID);
		}
	}
	else
	{
		ErrorString = qQuery.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISCore::DeleteCascadeObject(PMetaTable *MetaTable, int ObjectID, QString &ErrorString)
{
	ISQuery qDeleteCascade(QString("DELETE FROM %1 WHERE %2_id = :ObjectID").arg(MetaTable->Name).arg(MetaTable->Alias));
	qDeleteCascade.BindValue(":ObjectID", ObjectID);
	bool Result = qDeleteCascade.Execute();
	if (!Result)
	{
		ErrorString = qDeleteCascade.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------