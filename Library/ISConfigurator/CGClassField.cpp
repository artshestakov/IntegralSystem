#include "StdAfx.h"
#include "CGClassField.h"
#include "ISMetaData.h"
#include "ISQuery.h"
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
CGClassField::CGClassField() : QObject()
{

}
//-----------------------------------------------------------------------------
CGClassField::~CGClassField()
{

}
//-----------------------------------------------------------------------------
bool CGClassField::CheckExistClassField(PMetaClassField *MetaField)
{
	ISQuery qSelect(QS_CLASS_FIELD);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":UID", MetaField->GetUID());
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
	qInsert.BindValue(":UID", MetaField->GetUID());
	qInsert.BindValue(":ClassUID", ClassUID);
	qInsert.BindValue(":Name", MetaField->GetName());
	qInsert.BindValue(":Type", ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(MetaField->GetType()));
	qInsert.BindValue(":Size", MetaField->GetSize());
	qInsert.BindValue(":LabelName", MetaField->GetLabelName());
	qInsert.BindValue(":LocalListName", MetaField->GetLocalListName());
	qInsert.Execute();
}
//-----------------------------------------------------------------------------
void CGClassField::UpdateClassField(const ISUuid &ClassUID, PMetaClassField *MetaField)
{
	ISQuery qUpdate(QU_CLASS_FIELD);
	qUpdate.SetShowLongQuery(false);
	qUpdate.BindValue(":ClassUID", ClassUID);
	qUpdate.BindValue(":Name", MetaField->GetName());
	qUpdate.BindValue(":Type", ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(MetaField->GetType()));
	qUpdate.BindValue(":Size", MetaField->GetSize());
	qUpdate.BindValue(":LabelName", MetaField->GetLabelName());
	qUpdate.BindValue(":LocalListName", MetaField->GetLocalListName());
	qUpdate.BindValue(":UID", MetaField->GetUID());
	qUpdate.Execute();
}
//-----------------------------------------------------------------------------
