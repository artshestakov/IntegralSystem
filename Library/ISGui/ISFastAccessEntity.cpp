#include "StdAfx.h"
#include "ISFastAccessEntity.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISFastAccessEntity.h"
//-----------------------------------------------------------------------------
static QString QS_EXTERNAL_TOOLS = PREPARE_QUERY("SELECT extl_uid, extl_uid, extl_localname, extl_command, extl_icon FROM _externaltools WHERE extl_user = currentuserid() ORDER BY extl_order");
//-----------------------------------------------------------------------------
static QString QS_CREATE_RECORDS = PREPARE_QUERY("SELECT fcob_table FROM _fastcreateobject WHERE fcob_user = currentuserid() ORDER BY fcob_id");
//-----------------------------------------------------------------------------
ISFastAccessEntity::ISFastAccessEntity() : QObject()
{

}
//-----------------------------------------------------------------------------
ISFastAccessEntity::~ISFastAccessEntity()
{

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
			ISUuid UID = qSelect.ReadColumn("extl_uid").toString();
			QString LocalName = qSelect.ReadColumn("extl_localname").toString();
			QString Command = qSelect.ReadColumn("extl_command").toString();
			QIcon Icon = QIcon(ISSystem::ByteArrayToPixmap(qSelect.ReadColumn("extl_icon").toByteArray()));

			ISMetaExternalTool *MetaExternalTool = new ISMetaExternalTool(this);
			MetaExternalTool->SetUID(UID);
			MetaExternalTool->SetLocalName(LocalName);
			MetaExternalTool->SetCommand(Command);
			MetaExternalTool->SetIcon(Icon);
			ExternalTools.append(MetaExternalTool);
		}
	}
}
//-----------------------------------------------------------------------------
void ISFastAccessEntity::ReloadExternalTools()
{
	while (ExternalTools.count())
	{
		ISMetaExternalTool *MetaExternalTool = ExternalTools.takeFirst();

		delete MetaExternalTool;
		MetaExternalTool = nullptr;
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
			CreateRecords.append(qSelect.ReadColumn("fcob_table").toString());
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
QVectorString ISFastAccessEntity::GetCreateRecords()
{
	return CreateRecords;
}
//-----------------------------------------------------------------------------
