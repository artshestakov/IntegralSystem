#include "ISSettings.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISAssert.h"
#include "ISCountingTime.h"
#include "ISSystem.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
static QString QS_SETTINGS = PREPARE_QUERY("SELECT "
										   "stgp_uid, stgp_name, stgp_localname, stgp_iconname, stgp_hint, "
										   "stgs_uid, stgs_name, stgs_type, stgs_widgeteditname, stgs_localname, stgs_hint, stgs_defaultvalue, "
										   "usst_value, "
										   "(SELECT COUNT(*) FROM _usersettings WHERE usst_user = currentuserid() AND usst_setting = stgs_uid) "
										   "FROM _settings "
										   "LEFT JOIN _settingsgroup ON stgp_uid = stgs_group "
										   "LEFT JOIN _usersettings ON usst_setting = stgs_uid AND usst_user = currentuserid() "
										   "WHERE NOT stgs_isdeleted "
										   "ORDER BY stgp_order, stgs_order");
//-----------------------------------------------------------------------------
static QString QI_USER_SETTING = PREPARE_QUERY("INSERT INTO _usersettings(usst_user, usst_setting, usst_value) "
											   "VALUES(currentuserid(), :SettingUID, :Value)");
//-----------------------------------------------------------------------------
static QString QU_USER_SETTING_VALUE = PREPARE_QUERY("UPDATE _usersettings SET "
													 "usst_value = :Value "
													 "WHERE usst_user = currentuserid() "
													 "AND usst_setting = :SettingUID");
//-----------------------------------------------------------------------------
ISSettings::ISSettings()
{
	Initialize();
}
//-----------------------------------------------------------------------------
ISSettings::~ISSettings()
{
	while (!SettingGroups.isEmpty())
	{
		delete SettingGroups.takeLast();
	}
}
//-----------------------------------------------------------------------------
ISSettings& ISSettings::GetInstance()
{
	static ISSettings Settings;
	return Settings;
}
//-----------------------------------------------------------------------------
bool ISSettings::GetValueBool(const QString &SettingUID)
{
	return GetValue(SettingUID).toBool();
}
//-----------------------------------------------------------------------------
QString ISSettings::GetValueString(const QString &SettingUID)
{
	return GetValue(SettingUID).toString();
}
//-----------------------------------------------------------------------------
int ISSettings::GetValueInt(const QString &SettingUID)
{
	return GetValue(SettingUID).toInt();
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
}
//-----------------------------------------------------------------------------
QVector<ISMetaSettingsGroup*> ISSettings::GetSettingGroups()
{
	return SettingGroups;
}
//-----------------------------------------------------------------------------
ISMetaSetting* ISSettings::GetMetaSetting(const QString &SettingUID)
{
	for (int i = 0; i < SettingGroups.count(); ++i)
	{
		ISMetaSettingsGroup *MetaGroup = SettingGroups.at(i);
		for (int j = 0; j < MetaGroup->Settings.count(); ++j)
		{
			ISMetaSetting *MetaSetting = MetaGroup->Settings[j];
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
void ISSettings::SaveValue(const QString &SettingUID, const QVariant &Value)
{
	ISQuery qUpdateValue(QU_USER_SETTING_VALUE);
	qUpdateValue.BindValue(":Value", Value);
	qUpdateValue.BindValue(":SettingUID", SettingUID);
	if (qUpdateValue.Execute())
	{
		SetValue(SettingUID, Value);
	}
}
//-----------------------------------------------------------------------------
void ISSettings::Initialize()
{
	ISCountingTime Time;

	ISQuery qSelectSettings(QS_SETTINGS);
	if (qSelectSettings.Execute())
	{
		while (qSelectSettings.Next())
		{
			ISUuid GroupUID = qSelectSettings.ReadColumn("stgp_uid");
			ISMetaSettingsGroup *MetaGroup = CheckExistGroup(GroupUID);
			if (!MetaGroup) //Если группа настроек уже содержится в памяти (была создана при первой итерации обхода результатов запрос)
			{
				MetaGroup = new ISMetaSettingsGroup();
				MetaGroup->UID = GroupUID;
				MetaGroup->Name = qSelectSettings.ReadColumn("stgp_name").toString();
				MetaGroup->LocalName = qSelectSettings.ReadColumn("stgp_localname").toString();
				MetaGroup->IconName = qSelectSettings.ReadColumn("stgp_iconname").toString();
				MetaGroup->Hint = qSelectSettings.ReadColumn("stgp_hint").toString();
				SettingGroups.append(MetaGroup);
			}

			ISUuid SettingUID = qSelectSettings.ReadColumn("stgs_uid");

			ISMetaSetting *Setting = new ISMetaSetting();
			Setting->UID = SettingUID;
			Setting->Name = qSelectSettings.ReadColumn("stgs_name").toString();
			Setting->SettingType = ISMetaData::GetInstanse().GetTypeField(qSelectSettings.ReadColumn("stgs_type").toString());
			Setting->WidgetEditName = qSelectSettings.ReadColumn("stgs_widgeteditname").toString();
			Setting->LocalName = qSelectSettings.ReadColumn("stgs_localname").toString();
			Setting->Hint = qSelectSettings.ReadColumn("stgs_hint").toString();
			Setting->DefaultValue = qSelectSettings.ReadColumn("stgs_defaultvalue");
			MetaGroup->Settings.append(Setting);

			if (qSelectSettings.ReadColumn("count").toInt())
			{
				Setting->Value = qSelectSettings.ReadColumn("usst_value");
			}
			else //Если такой настройки нет у пользователя - добавить
			{
				QVariant SettingDefaultValue = qSelectSettings.ReadColumn("stgs_defaultvalue");
				InsertSetting(SettingUID, SettingDefaultValue);
				Setting->Value = SettingDefaultValue;
			}
		}
	}

	ISLOGGER_DEBUG(QString("Initialized Settings time %1 msec").arg(Time.Elapsed()));
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
void ISSettings::InsertSetting(const QString &SettingUID, const QVariant &Value)
{
	ISQuery qInsertSetting(QI_USER_SETTING);
	qInsertSetting.BindValue(":SettingUID", SettingUID);
	qInsertSetting.BindValue(":Value", Value);
	IS_ASSERT(qInsertSetting.Execute(), "Query for initialize new settings not executed. Query text: " + qInsertSetting.GetSqlText());
}
//-----------------------------------------------------------------------------
