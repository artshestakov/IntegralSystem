#include "ISFastAccessEntity.h"
#include "ISQuery.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_EXTERNAL_TOOLS = PREPARE_QUERY("SELECT extl_uid, extl_uid, extl_localname, extl_command, extl_icon "
												 "FROM _externaltools "
												 "WHERE extl_user = currentuserid() "
												 "ORDER BY extl_order");
//-----------------------------------------------------------------------------
static QString QS_CREATE_RECORDS = PREPARE_QUERY("SELECT fcob_table FROM _fastcreateobject WHERE fcob_user = currentuserid() ORDER BY fcob_id");
//-----------------------------------------------------------------------------
ISFastAccessEntity::ISFastAccessEntity()
{

}
//-----------------------------------------------------------------------------
ISFastAccessEntity::~ISFastAccessEntity()
{
	while (!ExternalTools.isEmpty())
	{
		delete ExternalTools.takeLast();
	}
}
//-----------------------------------------------------------------------------
ISFastAccessEntity& ISFastAccessEntity::GetInstance()
{
	static ISFastAccessEntity ExternalTools;
	return ExternalTools;
}
//-----------------------------------------------------------------------------
void ISFastAccessEntity::LoadExternalTools()
{
	ISQuery qSelect(QS_EXTERNAL_TOOLS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISMetaExternalTool *MetaExternalTool = new ISMetaExternalTool();
			MetaExternalTool->UID = qSelect.ReadColumn("extl_uid").toString();
			MetaExternalTool->LocalName = qSelect.ReadColumn("extl_localname").toString();
			MetaExternalTool->Command = qSelect.ReadColumn("extl_command").toString();
			MetaExternalTool->Icon = QIcon(ISGui::ByteArrayToPixmap(qSelect.ReadColumn("extl_icon").toByteArray()));
			ExternalTools.append(MetaExternalTool);
		}
	}
}
//-----------------------------------------------------------------------------
void ISFastAccessEntity::ReloadExternalTools()
{
	while (!ExternalTools.isEmpty())
	{
		delete ExternalTools.takeFirst();
	}
	LoadExternalTools();
}
//-----------------------------------------------------------------------------
QVector<ISMetaExternalTool*> ISFastAccessEntity::GetExternalTools()
{
	return ExternalTools;
}
//-----------------------------------------------------------------------------
void ISFastAccessEntity::LoadCreateRecords()
{
	ISQuery qSelect(QS_CREATE_RECORDS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			CreateRecords.emplace_back(qSelect.ReadColumn("fcob_table").toString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISFastAccessEntity::ReloadCreateRecords()
{
	CreateRecords.clear();
	LoadCreateRecords();
}
//-----------------------------------------------------------------------------
ISVectorString ISFastAccessEntity::GetCreateRecords()
{
	return CreateRecords;
}
//-----------------------------------------------------------------------------