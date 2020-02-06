#include "ISMetaSettingsGroup.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISMetaSettingsGroup::ISMetaSettingsGroup()
	: System(true)
{
	
}
//-----------------------------------------------------------------------------
ISMetaSettingsGroup::~ISMetaSettingsGroup()
{
	while (!Settings.isEmpty())
	{
		delete Settings.takeLast();
	}
}
//-----------------------------------------------------------------------------
void ISMetaSettingsGroup::SetName(const QString &name)
{
	Name = name;
}
//-----------------------------------------------------------------------------
QString ISMetaSettingsGroup::GetName() const
{
	return Name;
}
//-----------------------------------------------------------------------------
void ISMetaSettingsGroup::SetUID(const ISUuid &uid)
{
	UID = uid;
}
//-----------------------------------------------------------------------------
ISUuid ISMetaSettingsGroup::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
void ISMetaSettingsGroup::SetLocalName(const QString &local_name)
{
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSettingsGroup::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void ISMetaSettingsGroup::SetIconName(const QString &icon_name)
{
	IconName = icon_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSettingsGroup::GetIconName() const
{
	return IconName;
}
//-----------------------------------------------------------------------------
void ISMetaSettingsGroup::SetSystem(bool system)
{
	System = system;
}
//-----------------------------------------------------------------------------
bool ISMetaSettingsGroup::GetSystem() const
{
	return System;
}
//-----------------------------------------------------------------------------
void ISMetaSettingsGroup::SetHint(const QString &hint)
{
	Hint = hint;
}
//-----------------------------------------------------------------------------
QString ISMetaSettingsGroup::GetHint() const
{
	return Hint;
}
//-----------------------------------------------------------------------------
void ISMetaSettingsGroup::AppendSetting(ISMetaSetting *Setting)
{
	Settings.append(Setting);
}
//-----------------------------------------------------------------------------
QVector<ISMetaSetting*> ISMetaSettingsGroup::GetSettings()
{
	return Settings;
}
//-----------------------------------------------------------------------------
