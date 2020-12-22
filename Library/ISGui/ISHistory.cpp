#include "ISHistory.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISSettings.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
static QString QS_HISTORY = PREPARE_QUERY("SELECT htry_datetime, htry_tablename, htry_objectid "
										  "FROM _history "
										  "WHERE htry_user = :UserID "
										  "ORDER BY htry_id");
//-----------------------------------------------------------------------------
static QString QD_HISTORY = PREPARE_QUERY("DELETE FROM _history WHERE htry_user = :UserID");
//-----------------------------------------------------------------------------
static QString QI_HISTORY = PREPARE_QUERY("INSERT INTO _history(htry_user, htry_datetime, htry_tablename, htry_objectid) "
										  "VALUES(:UserID, :DateTime, :TableName, :ObjectID)");
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
void ISHistory::Initialize(const QVariantList &VariantList)
{
	for (const QVariant &History : VariantList)
	{
		QVariantMap HistoryMap = History.toMap();
		Stories.emplace_back(ISHistoryObject
		{
			HistoryMap["Date"].toDateTime(),
			HistoryMap["Table"].toString(),
			HistoryMap["ID"].toInt(),
			false
		});
	}
}
//-----------------------------------------------------------------------------
bool ISHistory::Initialize()
{
	ISQuery qSelect(QS_HISTORY);
	qSelect.BindValue(":UserID", CURRENT_USER_ID);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			Stories.emplace_back(ISHistoryObject
			{
				qSelect.ReadColumn("htry_datetime").toDateTime(),
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
	if (SETTING_BOOL(CONST_UID_SETTING_OTHER_ENABLE_HISTORY_TRACKING))
	{
		Stories.emplace_back(ISHistoryObject{ QDateTime::currentDateTime(), TableName, ObjectID, true });
	}
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
	if (Stories.empty()) //���� ������� �������� �������� ��� (�������� ������� �����) - ������� ��� ������� �� ��
	{
		ISQuery qDelete(QD_HISTORY);
		qDelete.BindValue(":UserID", CURRENT_USER_ID);
		Result = qDelete.Execute();
		if (!Result)
		{
			ErrorString = qDelete.GetErrorString();
		}
	}
	else //������� �������� �������� ���� - ���������
	{
		for (const ISHistoryObject &HistoryObject : Stories) //������� ��� �������
		{
			if (!HistoryObject.IsNew) //���� ������� ������ ������� �� ����� - ��������� � ����������
			{
				continue;
			}
			ISQuery qInsert(QI_HISTORY);
			qInsert.BindValue(":UserID", CURRENT_USER_ID);
			qInsert.BindValue(":DateTime", HistoryObject.DateTime);
			qInsert.BindValue(":TableName", HistoryObject.TableName);
			qInsert.BindValue(":ObjectID", HistoryObject.ObjectID);
			Result = qInsert.Execute();
			if (!Result) //�� ������� �������� ������� � �� - ��������� ����������
			{
				ErrorString = qInsert.GetErrorString();
				break;
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
