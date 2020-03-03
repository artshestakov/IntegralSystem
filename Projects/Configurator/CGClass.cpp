#include "CGClass.h"
#include "ISQuery.h"
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
	qSelect.BindValue(":Name", MetaTable->Name);
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
	qInsert.BindValue(":UID", MetaTable->UID);
	qInsert.BindValue(":IsSystem", MetaTable->IsSystem);
	qInsert.BindValue(":Name", MetaTable->Name);
	qInsert.BindValue(":Alias", MetaTable->Alias);
	qInsert.BindValue(":LocalName", MetaTable->LocalName);
	qInsert.BindValue(":LocalListName", MetaTable->LocalListName);
	qInsert.Execute();
}
//-----------------------------------------------------------------------------
void CGClass::UpdateClass(PMetaClassTable *MetaTable)
{
	ISQuery qUpdate(QU_CLASS);
	qUpdate.SetShowLongQuery(false);
	qUpdate.BindValue(":IsSystem", MetaTable->IsSystem);
	qUpdate.BindValue(":Name", MetaTable->Name);
	qUpdate.BindValue(":Alias", MetaTable->Alias);
	qUpdate.BindValue(":LocalName", MetaTable->LocalName);
	qUpdate.BindValue(":LocalListName", MetaTable->LocalListName);
	qUpdate.BindValue(":UID", MetaTable->UID);
	qUpdate.Execute();
}
//-----------------------------------------------------------------------------
