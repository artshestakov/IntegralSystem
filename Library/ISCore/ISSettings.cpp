#include "ISSettings.h"
#include "ISMetaData.h"
#include "ISAssert.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISSettings::ISSettings()
{
	
}
//-----------------------------------------------------------------------------
ISSettings::~ISSettings()
{
	while (!SettingGroups.empty())
	{
		delete ISAlgorithm::VectorTakeBack(SettingGroups);
	}
}
//-----------------------------------------------------------------------------
ISSettings& ISSettings::Instance()
{
	static ISSettings Settings;
	return Settings;
}
//-----------------------------------------------------------------------------
void ISSettings::Initialize(const QVariantList &VariantList)
{
	for (const QVariant &SettingGroup : VariantList)
	{
		QVariantMap GroupMap = SettingGroup.toMap();
		ISMetaSettingsGroup *MetaGroup = new ISMetaSettingsGroup();
		MetaGroup->UID = GroupMap["UID"].toString();
		MetaGroup->Name = GroupMap["Name"].toString();
		MetaGroup->LocalName = GroupMap["Local"].toString();
		MetaGroup->IconName = GroupMap["Icon"].toString();
		MetaGroup->Hint = GroupMap["Hint"].toString();

		QVariantList SettingsList = GroupMap["Settings"].toList();
		for (const QVariant &Setting : SettingsList)
		{
			QVariantMap SettingMap = Setting.toMap();
			ISMetaSetting *MetaSetting = new ISMetaSetting();
			MetaSetting->UID = SettingMap["UID"].toString();
			MetaSetting->Name = SettingMap["Name"].toString();
			MetaSetting->Type = ISMetaData::Instance().GetType(SettingMap["Type"].toString()).TypeField;
			MetaSetting->WidgetEditName = SettingMap["WidgetEditName"].toString();
			MetaSetting->LocalName = SettingMap["Local"].toString();
			MetaSetting->Hint = SettingMap["Hint"].toString();
			MetaSetting->Default = SettingMap["Default"];
			MetaSetting->Value = SettingMap["Value"];
			MetaGroup->Settings.emplace_back(MetaSetting);
		}
		SettingGroups.emplace_back(MetaGroup);
	}
}
//-----------------------------------------------------------------------------
QVariantMap ISSettings::GetSettingsChanged() const
{
	return SettingsChanged;
}
//-----------------------------------------------------------------------------
QVariant ISSettings::GetValue(const QString &SettingUID)
{
	return GetMetaSetting(SettingUID)->Value;
}
//-----------------------------------------------------------------------------
void ISSettings::SetValue(const QString &SettingUID, const QVariant &Value)
{
	GetMetaSetting(SettingUID)->Value = Value;
	SettingsChanged[SettingUID] = Value.toString();
}
//-----------------------------------------------------------------------------
std::vector<ISMetaSettingsGroup*> ISSettings::GetSettingGroups()
{
	return SettingGroups;
}
//-----------------------------------------------------------------------------
ISMetaSetting* ISSettings::GetMetaSetting(const QString &SettingUID)
{
	for (ISMetaSettingsGroup *MetaGroup : SettingGroups)
	{
		for (ISMetaSetting *MetaSetting : MetaGroup->Settings)
		{
			if (MetaSetting->UID == SettingUID)
			{
				return MetaSetting;
			}
		}
	}
	IS_ASSERT(false, QString("Not found setting \"%1\"").arg(SettingUID));
	return nullptr;
}
//-----------------------------------------------------------------------------
