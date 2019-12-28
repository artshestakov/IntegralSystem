#include "StdAfx.h"
#include "PMetaClassTable.h"
#include "ISDefines.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
PMetaClassTable::PMetaClassTable(QObject *parent) : PMetaClass(parent)
{
	UseRoles = true;
	ShowOnly = false;
	IsSystem = false;
}
//-----------------------------------------------------------------------------
PMetaClassTable::~PMetaClassTable()
{

}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetAlias(const QString &alias)
{
	IS_ASSERT(alias.length(), "Empty Alias in table");
	Alias = alias;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetAlias() const
{
	return Alias;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetLocalName(const QString &local_name)
{
	IS_ASSERT(local_name.length(), "Empty LocalName in table");
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetLocalListName(const QString &local_list_name)
{
	IS_ASSERT(local_list_name.length(), "Empty LocalListName in table");
	LocalListName = local_list_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetLocalListName() const
{
	return LocalListName;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetTitleName(const QString &title_name)
{
	TitleName = title_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetTitleName() const
{
	return TitleName;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetUseRoles(bool use_roles)
{
	UseRoles = use_roles;
}
//-----------------------------------------------------------------------------
bool PMetaClassTable::GetUseRoles() const
{
	return UseRoles;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetClassFilter(const QString &class_filter)
{
	ClassFilter = class_filter;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetClassFilter() const
{
	return ClassFilter;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetClassFilterField(const QString &class_filter_field)
{
	ClassFilterField = class_filter_field;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetClassFilterField() const
{
	return ClassFilterField;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetObjectForm(const QString &object_form)
{
	ObjectForm = object_form;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetObjectForm() const
{
	return ObjectForm;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetShowOnly(bool show_only)
{
	ShowOnly = show_only;
}
//-----------------------------------------------------------------------------
bool PMetaClassTable::GetShowOnly() const
{
	return ShowOnly;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetIsSystem(bool is_system)
{
	IsSystem = is_system;
}
//-----------------------------------------------------------------------------
bool PMetaClassTable::GetIsSystem() const
{
	return IsSystem;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetSqlModel(const QString &sql_model)
{
	SqlModel = sql_model;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetSqlModel() const
{
	return SqlModel;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetParent(const QString &parent)
{
	Parent = parent;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetParent() const
{
	return Parent;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetWhere(const QString &where)
{
	Where = where;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetWhere() const
{
	return Where;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::SetOrderField(const QString &order_field)
{
	OrderField = order_field;
}
//-----------------------------------------------------------------------------
QString PMetaClassTable::GetOrderField() const
{
	return OrderField;
}
//-----------------------------------------------------------------------------
void PMetaClassTable::AddField(PMetaClassField *Field)
{
	Fields.append(Field);
}
//-----------------------------------------------------------------------------
void PMetaClassTable::AddSystemField(PMetaClassField *SystemField)
{
	SystemFields.append(SystemField);
}
//-----------------------------------------------------------------------------
void PMetaClassTable::AddAllField(PMetaClassField *Field)
{
	AllFields.append(Field);
}
//-----------------------------------------------------------------------------
void PMetaClassTable::AddEscort(PMetaClassEscort *EscortObject)
{
	Escorts.append(EscortObject);
}
//-----------------------------------------------------------------------------
void PMetaClassTable::AddVisibleSystemField(PMetaClassField *MetaField)
{
	SystemFieldsVisible.append(MetaField);
}
//-----------------------------------------------------------------------------
void PMetaClassTable::AddJoin(const QString &JoinText)
{
	Joins.append(JoinText);
}
//-----------------------------------------------------------------------------
QVector<PMetaClassField*> PMetaClassTable::GetFields()
{
	return Fields;
}
//-----------------------------------------------------------------------------
QVector<PMetaClassField*> PMetaClassTable::GetSystemFields()
{
	return SystemFields;
}
//-----------------------------------------------------------------------------
QVector<PMetaClassField*> PMetaClassTable::GetAllFields()
{
	return AllFields;
}
//-----------------------------------------------------------------------------
QVector<PMetaClassEscort*> PMetaClassTable::GetEscorts()
{
	return Escorts;
}
//-----------------------------------------------------------------------------
QVector<PMetaClassField*> PMetaClassTable::GetVisibleSystemFields()
{
	return SystemFieldsVisible;
}
//-----------------------------------------------------------------------------
QVectorString PMetaClassTable::GetJoins()
{
	return Joins;
}
//-----------------------------------------------------------------------------
PMetaClassField* PMetaClassTable::GetField(const QString &FieldName)
{
	for (PMetaClassField *MetaField : AllFields)
	{
		if (MetaField->GetName().toLower() == FieldName.toLower())
		{
			return MetaField;
		}
	}

	return nullptr;
}
//-----------------------------------------------------------------------------
PMetaClassField* PMetaClassTable::GetField(int Index)
{
	return AllFields.at(Index);
}
//-----------------------------------------------------------------------------
PMetaClassField* PMetaClassTable::GetFieldID()
{
	if (SystemFields.at(0))
	{
		return SystemFields.at(0);
	}

	IS_ASSERT(false, QString("Not found ID field in meta table %1").arg(GetName()));
	return nullptr;
}
//-----------------------------------------------------------------------------
int PMetaClassTable::GetFieldIndex(const QString &FieldName) const
{
	for (int i = 0; i < AllFields.count(); i++)
	{
		PMetaClassField *MetaField = AllFields.at(i);
		if (MetaField->GetName() == FieldName)
		{
			return i;
		}
	}

	return -1;
}
//-----------------------------------------------------------------------------
