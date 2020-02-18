#include "CGClassField.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_CLASS_FIELD = PREPARE_QUERY("SELECT COUNT(*) FROM _classfield WHERE clsf_uid = :UID");
//-----------------------------------------------------------------------------
static QString QI_CLASS_FIELD = PREPARE_QUERY("INSERT INTO _classfield(clsf_uid, clsf_class, clsf_name, clsf_type, clsf_size, clsf_labelname, clsf_locallistname) "
											  "VALUES(:UID, (SELECT clss_id FROM _class WHERE clss_uid = :ClassUID), :Name, :Type, :Size, :LabelName, :LocalListName)");
//-----------------------------------------------------------------------------
static QString QU_CLASS_FIELD = PREPARE_QUERY("UPDATE _classfield SET "
											  "clsf_class = (SELECT clss_id FROM _class WHERE clss_uid = :ClassUID), "
											  "clsf_name = :Name, "
											  "clsf_type = :Type, "
											  "clsf_size = :Size, "
											  "clsf_labelname = :LabelName, "
											  "clsf_locallistname = :LocalListName "
											  "WHERE clsf_uid = :UID");
//-----------------------------------------------------------------------------
bool CGClassField::CheckExistClassField(PMetaClassField *MetaField)
{
	ISQuery qSelect(QS_CLASS_FIELD);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":UID", MetaField->UID);
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
void CGClassField::InsertClassField(const ISUuid &ClassUID, PMetaClassField *MetaField)
{
	ISQuery qInsert(QI_CLASS_FIELD);
	qInsert.SetShowLongQuery(false);
	qInsert.BindValue(":UID", MetaField->UID);
	qInsert.BindValue(":ClassUID", ClassUID);
	qInsert.BindValue(":Name", MetaField->Name);
	qInsert.BindValue(":Type", ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(MetaField->Type));
	qInsert.BindValue(":Size", MetaField->Size);
	qInsert.BindValue(":LabelName", MetaField->LabelName);
	qInsert.BindValue(":LocalListName", MetaField->LocalListName);
	qInsert.Execute();
}
//-----------------------------------------------------------------------------
void CGClassField::UpdateClassField(const ISUuid &ClassUID, PMetaClassField *MetaField)
{
	ISQuery qUpdate(QU_CLASS_FIELD);
	qUpdate.SetShowLongQuery(false);
	qUpdate.BindValue(":ClassUID", ClassUID);
	qUpdate.BindValue(":Name", MetaField->Name);
	qUpdate.BindValue(":Type", ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(MetaField->Type));
	qUpdate.BindValue(":Size", MetaField->Size);
	qUpdate.BindValue(":LabelName", MetaField->LabelName);
	qUpdate.BindValue(":LocalListName", MetaField->LocalListName);
	qUpdate.BindValue(":UID", MetaField->UID);
	qUpdate.Execute();
}
//-----------------------------------------------------------------------------
