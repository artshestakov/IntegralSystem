#include "CGClass.h"
#include "ISQuery.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_CLASS = PREPARE_QUERY("SELECT COUNT(*) FROM _class WHERE clss_name = :Name");
//-----------------------------------------------------------------------------
static QString QI_CLASS = PREPARE_QUERY("INSERT INTO _class(clss_uid, clss_issystem, clss_name, clss_alias, clss_localname, clss_locallistname) "
										"VALUES(:UID, :IsSystem, :Name, :Alias, :LocalName, :LocalListName)");
//-----------------------------------------------------------------------------
static QString QU_CLASS = PREPARE_QUERY("UPDATE _class SET "
										"clss_issystem = :IsSystem, "
										"clss_name = :Name, "
										"clss_alias = :Alias, "
										"clss_localname = :LocalName, "
										"clss_locallistname = :LocalListName "
										"WHERE clss_uid = :UID");
//-----------------------------------------------------------------------------
bool CGClass::CheckExistClass(PMetaClassTable *MetaTable)
{
	ISQuery qSelect(QS_CLASS);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":Name", MetaTable->GetName());
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("count").toInt())
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
void CGClass::InsertClass(PMetaClassTable *MetaTable)
{
	ISQuery qInsert(QI_CLASS);
	qInsert.SetShowLongQuery(false);
	qInsert.BindValue(":UID", MetaTable->GetUID());
	qInsert.BindValue(":IsSystem", MetaTable->GetIsSystem());
	qInsert.BindValue(":Name", MetaTable->GetName());
	qInsert.BindValue(":Alias", MetaTable->GetAlias());
	qInsert.BindValue(":LocalName", MetaTable->GetLocalName());
	qInsert.BindValue(":LocalListName", MetaTable->GetLocalListName());
	qInsert.Execute();
}
//-----------------------------------------------------------------------------
void CGClass::UpdateClass(PMetaClassTable *MetaTable)
{
	ISQuery qUpdate(QU_CLASS);
	qUpdate.SetShowLongQuery(false);
	qUpdate.BindValue(":IsSystem", MetaTable->GetIsSystem());
	qUpdate.BindValue(":Name", MetaTable->GetName());
	qUpdate.BindValue(":Alias", MetaTable->GetAlias());
	qUpdate.BindValue(":LocalName", MetaTable->GetLocalName());
	qUpdate.BindValue(":LocalListName", MetaTable->GetLocalListName());
	qUpdate.BindValue(":UID", MetaTable->GetUID());
	qUpdate.Execute();
}
//-----------------------------------------------------------------------------
