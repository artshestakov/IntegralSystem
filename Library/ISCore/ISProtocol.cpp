#include "ISProtocol.h"
#include "ISAssert.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISConstants.h"
#include "ISQueryPool.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
static QString QI_PROTOCOL = PREPARE_QUERY("INSERT INTO _protocol(prtc_creationdate, prtc_creationuser, prtc_tablename, prtc_tablelocalname, prtc_type, prtc_objectid, prtc_information) "
										   "VALUES(:CreationDate, :CreationUser, :TableName, :TableLocalName, (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = :TypeUID), :ObjectID, :Information) "
										   "RETURNING prtc_id");
//-----------------------------------------------------------------------------
void ISProtocol::EnterApplication()
{
	Insert(true, CONST_UID_PROTOCOL_ENTER_APPLICATION, QString(), QString(), QVariant());
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
void ISProtocol::Insert(bool Thread, const QString &TypeUID, const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &Information)
{
	if (Thread)
	{
		ISQueryPool::Instance().AddQuery(QI_PROTOCOL,
		{
			{ ":CreationDate", QDateTime::currentDateTime() },
			{ ":CreationUser", CURRENT_USER_ID },
			{ ":TypeUID", TypeUID },
			{ ":TableName", TableName },
			{ ":ObjectID", ObjectID },
			{ ":TableLocalName", LocalListName },
			{ ":Information", Information }
		});
	}
	else
	{
		ISQuery qInsertProtocol(QI_PROTOCOL);
		qInsertProtocol.BindValue(":CreationDate", QDateTime::currentDateTime());
		qInsertProtocol.BindValue(":CreationUser", CURRENT_USER_ID);
		qInsertProtocol.BindValue(":TableName", TableName);
		qInsertProtocol.BindValue(":TypeUID", TypeUID);
		qInsertProtocol.BindValue(":ObjectID", ObjectID);
		qInsertProtocol.BindValue(":TableLocalName", LocalListName);
		qInsertProtocol.BindValue(":Information", Information);
		if (!qInsertProtocol.ExecuteFirst())
		{
			ISLOGGER_E("Error insert protocol: " + qInsertProtocol.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
