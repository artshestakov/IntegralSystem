#include "ISHistory.h"
#include "ISConstants.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_HISTORY = PREPARE_QUERY("SELECT htry_creationdate, htry_tablename, htry_objectid "
										  "FROM _history "
										  "WHERE htry_user = currentuserid() "
										  "ORDER BY htry_id");
//-----------------------------------------------------------------------------
static QString QD_HISTORY = PREPARE_QUERY("DELETE FROM _history WHERE htry_user = currentuserid()");
//-----------------------------------------------------------------------------
static QString QI_HISTORY = PREPARE_QUERY("INSERT INTO _history(htry_creationdate, htry_tablename, htry_objectid) "
										  "VALUES(:CreationDate, :TableName, :ObjectID)");
//-----------------------------------------------------------------------------
ISHistory::ISHistory()
	: ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISHistory::~ISHistory()
{

}
//-----------------------------------------------------------------------------
ISHistory& ISHistory::Instance()
{
	static ISHistory History;
	return History;
}
//-----------------------------------------------------------------------------
QString ISHistory::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISHistory::Initialize()
{
	ISQuery qSelect(QS_HISTORY);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			Stories.emplace_back(ISHistoryObject
			{
				qSelect.ReadColumn("htry_creationdate").toDateTime(),
				qSelect.ReadColumn("htry_tablename").toString(),
				qSelect.ReadColumn("htry_objectid").toInt(),
				false
			});
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISHistory::AddObject(const QString &TableName, int ObjectID)
{
	Stories.emplace_back(ISHistoryObject{ QDateTime::currentDateTime(), TableName, ObjectID, true });
}
//-----------------------------------------------------------------------------
void ISHistory::Clear()
{
	Stories.clear();
}
//-----------------------------------------------------------------------------
std::vector<ISHistoryObject>& ISHistory::GetHistory()
{
	return Stories;
}
//-----------------------------------------------------------------------------
bool ISHistory::Save()
{
	bool Result = true;
	if (Stories.empty()) //Если истории открытых объектов нет (возможно удалили ранее) - удаляем всю историю из БД
	{
		ISQuery qDelete(QD_HISTORY);
		Result = qDelete.Execute();
		if (!Result)
		{
			ErrorString = qDelete.GetErrorString();
		}
	}
	else //История открытых объектов есть - сохраняем
	{
		for (const ISHistoryObject &HistoryObject : Stories) //Обходим всю историю
		{
			if (!HistoryObject.IsNew) //Если текущий объект истории не новый - переходим к следующему
			{
				continue;
			}
			ISQuery qInsert(QI_HISTORY);
			qInsert.BindValue(":CreationDate", HistoryObject.DateTime);
			qInsert.BindValue(":TableName", HistoryObject.TableName);
			qInsert.BindValue(":ObjectID", HistoryObject.ObjectID);
			Result = qInsert.Execute();
			if (!Result) //Не удалось добавить историю в БД - прерываем сохранение
			{
				ErrorString = qInsert.GetErrorString();
				break;
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
