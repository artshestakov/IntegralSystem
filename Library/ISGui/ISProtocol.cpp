#include "ISProtocol.h"
#include "ISQuery.h"
#include "ISConstants.h"
#include "ISQueryPool.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
static QString QI_PROTOCOL = PREPARE_QUERY("INSERT INTO _protocol(prtc_datetime, prtc_user, prtc_tablename, prtc_tablelocalname, prtc_type, prtc_objectid, prtc_information) "
										   "VALUES(:DateTime, :UserID, :TableName, :TableLocalName, (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = :TypeUID), :ObjectID, :Information)");
//-----------------------------------------------------------------------------
void ISProtocol::CreateObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName)
{
	Insert(CONST_UID_PROTOCOL_CREATE_OBJECT, TableName, LocalListName, ObjectID, ObjectName);
}
//-----------------------------------------------------------------------------
void ISProtocol::CreateCopyObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName)
{
	Insert(CONST_UID_PROTOCOL_CREATE_COPY_OBJECT, TableName, LocalListName, ObjectID, ObjectName);
}
//-----------------------------------------------------------------------------
void ISProtocol::EditObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName)
{
	Insert(CONST_UID_PROTOCOL_EDIT_OBJECT, TableName, LocalListName, ObjectID, ObjectName);
}
//-----------------------------------------------------------------------------
void ISProtocol::Insert(const QString &TypeUID, const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &Information)
{
	ISQueryPool::Instance().AddQuery(QI_PROTOCOL,
	{
		{ ":DateTime", QDateTime::currentDateTime() },
		{ ":UserID", CURRENT_USER_ID },
		{ ":TypeUID", TypeUID },
		{ ":TableName", TableName },
		{ ":ObjectID", ObjectID },
		{ ":TableLocalName", LocalListName },
		{ ":Information", Information }
	});
}
//-----------------------------------------------------------------------------
