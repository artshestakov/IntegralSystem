#include "PMetaClassField.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
PMetaClassField::PMetaClassField()
	: PMetaClass("Field"),
	Sequence(false),
	Type(ISNamespace::FT_Unknown),
	Size(0),
	Upper(false),
	Lower(false),
	NotNull(false),
	ReadOnly(false),
	HideFromObject(false),
	HideFromList(false),
	NotSearch(false),
	IsSystem(false),
	Index(nullptr),
	Foreign(nullptr)
{

}
//-----------------------------------------------------------------------------
PMetaClassField::~PMetaClassField()
{

}
//-----------------------------------------------------------------------------
void PMetaClassField::SetUID(const ISUuid &uid)
{
	UID = uid;
}
//-----------------------------------------------------------------------------
ISUuid PMetaClassField::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetName(const QString &name)
{
	Name = name;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetName() const
{
	return Name;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetType(const QString &type)
{
	Type = ISMetaData::GetInstanse().GetAssociationTypes().GetTypeField(type);
}
//-----------------------------------------------------------------------------
ISNamespace::FieldType PMetaClassField::GetType()
{
	return Type;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetSize(int size)
{
	Size = size;
}
//-----------------------------------------------------------------------------
int PMetaClassField::GetSize() const
{
	return Size;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetUpper(bool upper)
{
	Upper = upper;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::GetUpper() const
{
	return Upper;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetLower(bool lower)
{
	Lower = lower;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::GetLower() const
{
	return Lower;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetDefaultValue(const QVariant &default_value)
{
	DefaultValue = default_value;
}
//-----------------------------------------------------------------------------
QVariant PMetaClassField::GetDefaultValue() const
{
	return DefaultValue;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetDefaultValueWidget(const QVariant &default_value_widget)
{
	DefaultValueWidget = default_value_widget;
}
//-----------------------------------------------------------------------------
QVariant PMetaClassField::GetDefaultValueWidget() const
{
	return DefaultValueWidget;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetLabelName(const QString &label_name)
{
	LabelName = label_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetLabelName() const
{
	return LabelName;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetLocalListName(const QString &local_list_name)
{
	LocalListName = local_list_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetLocalListName() const
{
	return LocalListName;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetNotNull(bool not_null)
{
	NotNull = not_null;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::GetNotNull() const
{
	return NotNull;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetReadOnly(bool read_only)
{
	ReadOnly = read_only;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::GetReadOnly() const
{
	return ReadOnly;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetHideFromObject(bool hide_from_object)
{
	HideFromObject = hide_from_object;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::GetHideFromObject() const
{
	return HideFromObject;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetHideFromList(bool hide_from_list)
{
	HideFromList = hide_from_list;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::GetHideFromList() const
{
	return HideFromList;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetNotSearch(bool not_search)
{
	NotSearch = not_search;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::GetNotSearch() const
{
	return NotSearch;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetHint(const QString &hint)
{
	Hint = hint;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetHint() const
{
	return Hint;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetPlaceholderText(const QString &placeholder_text)
{
	PlaceholderText = placeholder_text;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetPlaceholderText() const
{
	return PlaceholderText;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetControlWidget(const QString &control_widget)
{
	ControlWidget = control_widget;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetControlWidget() const
{
	return ControlWidget;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetRegExp(const QString &reg_exp)
{
	RegExp = reg_exp;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetRegExp() const
{
	return RegExp;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetIsSystem(bool is_system)
{
	IsSystem = is_system;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::GetIsSystem() const
{
	return IsSystem;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetQueryText(const QString &query_text)
{
	QueryText = query_text;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetQueryText() const
{
	return QueryText;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetForeign(PMetaClassForeign *foreign)
{
	Foreign = foreign;
}
//-----------------------------------------------------------------------------
PMetaClassForeign* PMetaClassField::GetForeign()
{
	return Foreign;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetIndex(PMetaClassIndex *index)
{
	Index = index;
}
//-----------------------------------------------------------------------------
PMetaClassIndex* PMetaClassField::GetIndex()
{
	return Index;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetSequence(bool sequence)
{
	Sequence = sequence;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::GetSequence() const
{
	return Sequence;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetLayoutName(const QString &layout_name)
{
	LayoutName = layout_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetLayoutName() const
{
	return LayoutName;
}
//-----------------------------------------------------------------------------
void PMetaClassField::SetSeparatorName(const QString &separator_name)
{
	SeparatorName = separator_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassField::GetSeparatorName() const
{
	return SeparatorName;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::IsFieldID() const
{
	if (GetName() == "ID")
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
bool PMetaClassField::IsFieldUID() const
{
	if (GetName() == "UID")
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
