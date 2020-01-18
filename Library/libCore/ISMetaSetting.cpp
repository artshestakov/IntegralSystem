#include "ISMetaSetting.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISMetaSetting::ISMetaSetting(QObject *parent) : QObject(parent)
{
	SettingType = ISNamespace::FT_Unknown;
}
//-----------------------------------------------------------------------------
ISMetaSetting::~ISMetaSetting()
{

}
//-----------------------------------------------------------------------------
void ISMetaSetting::SetUID(const ISUuid &uid)
{
	UID = uid;
}
//-----------------------------------------------------------------------------
ISUuid ISMetaSetting::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
void ISMetaSetting::SetName(const QString &name)
{
	Name = name;
}
//-----------------------------------------------------------------------------
QString ISMetaSetting::GetName() const
{
	return Name;
}
//-----------------------------------------------------------------------------
void ISMetaSetting::SetType(ISNamespace::FieldType setting_type)
{
	SettingType = setting_type;
}
//-----------------------------------------------------------------------------
ISNamespace::FieldType ISMetaSetting::GetType() const
{
	return SettingType;
}
//-----------------------------------------------------------------------------
void ISMetaSetting::SetWidgetEditName(const QString &widget_edit_name)
{
	WidgetEditName = widget_edit_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSetting::GetWidgetEditName() const
{
	return WidgetEditName;
}
//-----------------------------------------------------------------------------
void ISMetaSetting::SetLocalName(const QString &local_name)
{
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSetting::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void ISMetaSetting::SetHint(const QString &hint)
{
	Hint = hint;
}
//-----------------------------------------------------------------------------
QString ISMetaSetting::GetHint() const
{
	return Hint;
}
//-----------------------------------------------------------------------------
void ISMetaSetting::SetDefaultValue(const QVariant &default_value)
{
	DefaultValue = default_value;
}
//-----------------------------------------------------------------------------
QVariant ISMetaSetting::GetDefaultValue() const
{
	return DefaultValue;
}
//-----------------------------------------------------------------------------
void ISMetaSetting::SetValue(const QVariant &value)
{
	Value = value;
}
//-----------------------------------------------------------------------------
QVariant ISMetaSetting::GetValue() const
{
	return Value;
}
//-----------------------------------------------------------------------------
