#include "ISProtocol.h"
#include "ISAssert.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISConstants.h"
#include "ISQueryPool.h"
//-----------------------------------------------------------------------------
static QString QI_PROTOCOL = PREPARE_QUERY("INSERT INTO _protocol(prtc_tablename, prtc_tablelocalname, prtc_type, prtc_objectid, prtc_information) "
										   "VALUES(:TableName, :TableLocalName, (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = :TypeUID), :ObjectID, :Information) "
										   "RETURNING prtc_id");
//-----------------------------------------------------------------------------
static QString QU_USER_LAST_ENTER = PREPARE_QUERY("UPDATE _users SET "
												  "usrs_lastenter = now(), "
												  "usrs_lastaddress = split_part(inet_client_addr()::VARCHAR, '/', 1) "
												  "WHERE usrs_id = currentuserid()");
//-----------------------------------------------------------------------------
void ISProtocol::EnterApplication()
{
	int ProtocolID = Insert(false, CONST_UID_PROTOCOL_ENTER_APPLICATION, QString(), QString(), QVariant());
	if (ProtocolID)
	{
		ISQuery qLastEnter(QU_USER_LAST_ENTER);
		qLastEnter.Execute();
	}
}
//-----------------------------------------------------------------------------
void ISProtocol::ExitApplication()
{
	Insert(false, CONST_UID_PROTOCOL_EXIT_APPLICATION, QString(), QString(), QString());
}
//-----------------------------------------------------------------------------
void ISProtocol::OpenSubSystem(const QString &TableName, const QString &LocalListName)
{
	Insert(true, CONST_UID_PROTOCOL_OPEN_SUB_SYSTEM, TableName, LocalListName, QVariant());
}
//-----------------------------------------------------------------------------
void ISProtocol::CreateObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName)
{
	Insert(true, CONST_UID_PROTOCOL_CREATE_OBJECT, TableName, LocalListName, ObjectID, ObjectName);
}
//-----------------------------------------------------------------------------
void ISProtocol::CreateCopyObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName)
{
	Insert(true, CONST_UID_PROTOCOL_CREATE_COPY_OBJECT, TableName, LocalListName, ObjectID, ObjectName);
}
//-----------------------------------------------------------------------------
void ISProtocol::EditObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName)
{
	Insert(true, CONST_UID_PROTOCOL_EDIT_OBJECT, TableName, LocalListName, ObjectID, ObjectName);
}
//-----------------------------------------------------------------------------
void ISProtocol::ShowObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName)
{
	Insert(true, CONST_UID_PROTOCOL_SHOW_OBJECT, TableName, LocalListName, ObjectID, ObjectName);
}
//-----------------------------------------------------------------------------
void ISProtocol::DeleteObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID)
{
	Insert(true, CONST_UID_PROTOCOL_DELETE_OBJECT, TableName, LocalListName, ObjectID);
}
//-----------------------------------------------------------------------------
void ISProtocol::RecoveryObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID)
{
	Insert(true, CONST_UID_PROTOCOL_RECOVERY_OBJECT, TableName, LocalListName, ObjectID);
}
//-----------------------------------------------------------------------------
void ISProtocol::DeleteCascadeObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID)
{
	Insert(true, CONST_UID_PROTOCOL_DELETE_CASCADE_OBJECT, TableName, LocalListName, ObjectID);
}
//-----------------------------------------------------------------------------
int ISProtocol::Insert(bool Thread, const QString &TypeUID, const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &Information)
{
	int Result = 0;

	if (Thread)
	{
		QVariantMap Parameters;
		Parameters.insert(":TypeUID", TypeUID);
		Parameters.insert(":TableName", TableName);
		Parameters.insert(":ObjectID", ObjectID);
		Parameters.insert(":TableLocalName", LocalListName);
		Parameters.insert(":Information", Information);
		
		ISQueryPool::GetInstance().AddQuery(QI_PROTOCOL, Parameters);
	}
	else
	{
		ISQuery qInsertProtocol(QI_PROTOCOL);
		qInsertProtocol.BindValue(":TableName", TableName);
		qInsertProtocol.BindValue(":TypeUID", TypeUID);
		qInsertProtocol.BindValue(":ObjectID", ObjectID);
		qInsertProtocol.BindValue(":TableLocalName", LocalListName);
		qInsertProtocol.BindValue(":Information", Information);
		if (qInsertProtocol.ExecuteFirst())
		{
			Result = qInsertProtocol.ReadColumn("prtc_id").toInt();
		}
		else
		{
			IS_ASSERT(false, "Not executed query: " + qInsertProtocol.GetSqlText());
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
