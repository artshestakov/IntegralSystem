#include "ISSettings.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISAssert.h"
#include "ISAlgorithm.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
static QString QS_SETTINGS = PREPARE_QUERY("SELECT "
										   "stgp_uid, stgp_name, stgp_localname, stgp_iconname, stgp_hint, "
										   "stgs_uid, stgs_name, stgs_type, stgs_widgeteditname, stgs_localname, stgs_hint, stgs_defaultvalue, "
										   "usst_value, "
										   "(SELECT COUNT(*) FROM _usersettings WHERE usst_user = :UserID AND usst_setting = stgs_id) "
										   "FROM _settings "
										   "LEFT JOIN _settingsgroup ON stgp_uid = stgs_group "
										   "LEFT JOIN _usersettings ON usst_setting = stgs_id AND usst_user = :UserID "
										   "ORDER BY stgp_order, stgs_order");
//-----------------------------------------------------------------------------
static QString QI_USER_SETTING = PREPARE_QUERY("INSERT INTO _usersettings(usst_user, usst_setting, usst_value) "
											   "VALUES(:UserID, (SELECT stgs_id FROM _settings WHERE stgs_uid = :SettingUID), :Value)");
//-----------------------------------------------------------------------------
static QString QU_USER_SETTING = PREPARE_QUERY("UPDATE _usersettings SET "
											   "usst_value = :Value "
											   "WHERE usst_user = :UserID "
											   "AND usst_setting = (SELECT stgs_id FROM _settings WHERE stgs_uid = :SettingUID)");
//-----------------------------------------------------------------------------
ISSettings::ISSettings()
	: ErrorString(NO_ERROR_STRING)
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
QString ISSettings::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISSettings::Initialize(const QVariantList &VariantList)
{
	for (const QVariant &SettingGroup : VariantList)
	{
		QVariantMap GroupMap = SettingGroup.toMap();
		ISMetaSettingsGroup *MetaGroup = new ISMetaSettingsGroup();
		MetaGroup->UID = GroupMap["UID"];
		MetaGroup->Name = GroupMap["Name"].toString();
		MetaGroup->LocalName = GroupMap["Local"].toString();
		MetaGroup->IconName = GroupMap["Icon"].toString();
		MetaGroup->Hint = GroupMap["Hint"].toString();

		QVariantList SettingsList = GroupMap["Settings"].toList();
		for (const QVariant &Setting : SettingsList)
		{
			QVariantMap SettingMap = Setting.toMap();
			ISMetaSetting *MetaSetting = new ISMetaSetting();
			MetaSetting->UID = SettingMap["UID"];
			MetaSetting->Name = SettingMap["Name"].toString();
			MetaSetting->Type = ISMetaData::Instance().GetTypeField(SettingMap["Type"].toString());
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
bool ISSettings::Initialize()
{
	ISQuery qSelectSettings(QS_SETTINGS);
	qSelectSettings.BindValue(":UserID", CURRENT_USER_ID);
	bool Result = qSelectSettings.Execute();
	if (Result)
	{
		while (qSelectSettings.Next())
		{
			ISUuid GroupUID = qSelectSettings.ReadColumn("stgp_uid");
			ISMetaSettingsGroup *MetaGroup = CheckExistGroup(GroupUID);
			if (!MetaGroup) //Если группа настроек уже содержится в памяти (была создана при первой итерации обхода результатов запроса)
			{
				MetaGroup = new ISMetaSettingsGroup();
				MetaGroup->UID = GroupUID;
				MetaGroup->Name = qSelectSettings.ReadColumn("stgp_name").toString();
				MetaGroup->LocalName = qSelectSettings.ReadColumn("stgp_localname").toString();
				MetaGroup->IconName = qSelectSettings.ReadColumn("stgp_iconname").toString();
				MetaGroup->Hint = qSelectSettings.ReadColumn("stgp_hint").toString();
				SettingGroups.emplace_back(MetaGroup);
			}

			ISUuid SettingUID = qSelectSettings.ReadColumn("stgs_uid");

			ISMetaSetting *Setting = new ISMetaSetting();
			Setting->UID = SettingUID;
			Setting->Name = qSelectSettings.ReadColumn("stgs_name").toString();
			Setting->Type = ISMetaData::Instance().GetTypeField(qSelectSettings.ReadColumn("stgs_type").toString());
			Setting->WidgetEditName = qSelectSettings.ReadColumn("stgs_widgeteditname").toString();
			Setting->LocalName = qSelectSettings.ReadColumn("stgs_localname").toString();
			Setting->Hint = qSelectSettings.ReadColumn("stgs_hint").toString();
			Setting->Default = qSelectSettings.ReadColumn("stgs_defaultvalue");
			MetaGroup->Settings.emplace_back(Setting);

			if (qSelectSettings.ReadColumn("count").toInt())
			{
				Setting->Value = qSelectSettings.ReadColumn("usst_value");
			}
			else //Если такой настройки нет у пользователя - добавить
			{
				QVariant SettingDefaultValue = qSelectSettings.ReadColumn("stgs_defaultvalue");
				Result = InsertSetting(SettingUID, SettingDefaultValue);
				if (!Result)
				{
					break;
				}
				Setting->Value = SettingDefaultValue;
			}
		}
	}
	else
	{
		ErrorString = qSelectSettings.GetErrorString();
	}
	return Result;
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
	SettingsChanged[SettingUID] = Value;
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
bool ISSettings::Save()
{
	bool Result = true;
	for (const auto &MapItem : SettingsChanged)
	{
		ISQuery qUpdateValue(QU_USER_SETTING);
		qUpdateValue.BindValue(":UserID", CURRENT_USER_ID);
		qUpdateValue.BindValue(":Value", MapItem.second);
		qUpdateValue.BindValue(":SettingUID", MapItem.first);
		Result = qUpdateValue.Execute();
		if (!Result)
		{
			ErrorString = qUpdateValue.GetErrorString();
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
ISMetaSettingsGroup* ISSettings::CheckExistGroup(const ISUuid &GroupUID)
{
	for (ISMetaSettingsGroup *MetaGriup : SettingGroups)
	{
		if (MetaGriup->UID == GroupUID)
		{
			return MetaGriup;
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
bool ISSettings::InsertSetting(const ISUuid &SettingUID, const QVariant &Value)
{
	ISQuery qInsertSetting(QI_USER_SETTING);
	qInsertSetting.BindValue(":UserID", CURRENT_USER_ID);
	qInsertSetting.BindValue(":SettingUID", SettingUID);
	qInsertSetting.BindValue(":Value", Value);
	bool Result = qInsertSetting.Execute();
	if (!Result)
	{
		ErrorString = "Error inserting new user setting: " + qInsertSetting.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
