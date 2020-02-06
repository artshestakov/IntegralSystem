#include "ISAssociationTypes.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISAssociationTypes::ISAssociationTypes()
{

}
//-----------------------------------------------------------------------------
ISAssociationTypes::~ISAssociationTypes()
{

}
//-----------------------------------------------------------------------------
void ISAssociationTypes::Insert(const QString &type_name, ISNamespace::FieldType meta_field_type, const QString &database_type, const QString &control_widget, const QString &search_operator_widget, bool search)
{
	IS_ASSERT(type_name.length(), "Type name from association type is null.");
	IS_ASSERT(database_type.length(), QString("Invalid database type from association type name: %1").arg(type_name));

	ISMetaDataType MetaType;
	MetaType.TypeName = type_name;
	MetaType.TypeField = meta_field_type;
	MetaType.TypeDB = database_type;
	MetaType.ControlWidget = control_widget;
	MetaType.SearchOperatorWidget = search_operator_widget;
	MetaType.Search = search;
	Types.append(MetaType);
}
//-----------------------------------------------------------------------------
ISNamespace::FieldType ISAssociationTypes::GetTypeField(const QString &type_name)
{
	for (const ISMetaDataType &Type : Types)
	{
		if (Type.TypeName == type_name)
		{
			return Type.TypeField;
		}
	}

	IS_ASSERT(false, QString("Not found field type from string: %1").arg(type_name));
	return ISNamespace::FT_Unknown;
}
//-----------------------------------------------------------------------------
QString ISAssociationTypes::GetTypeDB(ISNamespace::FieldType field_type) const
{
	for (const ISMetaDataType &Type : Types)
	{
		if (Type.TypeField == field_type)
		{
			return Type.TypeDB;
		}
	}

	IS_ASSERT(false, "Not found type db.");
	return QString();
}
//-----------------------------------------------------------------------------
QString ISAssociationTypes::GetTypeDB(const QString &type_name) const
{
	for (const ISMetaDataType &Type : Types)
	{
		if (Type.TypeName == type_name)
		{
			return Type.TypeDB;
		}
	}

	IS_ASSERT(false, QString("Not found type db: %1").arg(type_name));
	return QString();
}
//-----------------------------------------------------------------------------
QString ISAssociationTypes::GetControlWidgetFromType(ISNamespace::FieldType field_type) const
{
	for (const ISMetaDataType &Type : Types)
	{
		if (Type.TypeField == field_type)
		{
			return Type.ControlWidget;
		}
	}

	IS_ASSERT(false, "Not found control widget from field type.");
	return QString();
}
//-----------------------------------------------------------------------------
QString ISAssociationTypes::GetSearchOperatorWidget(ISNamespace::FieldType field_type) const
{
	for (const ISMetaDataType &Type : Types)
	{
		if (Type.TypeField == field_type)
		{
			return Type.SearchOperatorWidget;
		}
	}

	IS_ASSERT(false, "Not found search operator widget from field type.");
	return QString();
}
//-----------------------------------------------------------------------------
bool ISAssociationTypes::GetSearch(ISNamespace::FieldType field_type) const
{
	for (const ISMetaDataType &Type : Types)
	{
		if (Type.TypeField == field_type)
		{
			return Type.Search;
		}
	}

	IS_ASSERT(false, "Not found search permission from field type.");
	return false;
}
//-----------------------------------------------------------------------------
