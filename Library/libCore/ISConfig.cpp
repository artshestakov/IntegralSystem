#include "ISConfig.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISDebug.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: ErrorString("No error."),
	Settings(new QSettings(PATH_CONFIG_FILE, QSettings::IniFormat)),
	PathConfigTemplate(":/ConfigTemplate/" + APPLICATION_NAME + SYMBOL_POINT + EXTENSION_INI)
{
	
}
//-----------------------------------------------------------------------------
ISConfig::~ISConfig()
{
	if (Settings)
	{
		delete Settings;
	}
}
//-----------------------------------------------------------------------------
ISConfig& ISConfig::GetInstance()
{
	static ISConfig Config;
	return Config;
}
//-----------------------------------------------------------------------------
void ISConfig::Initialize()
{
	if (QFile::exists(PATH_CONFIG_FILE)) //���� ���������������� ���� ���������� - ������ ��� � ������ � ��������� ������������� ����������
	{
		Update();
	}
	else //���������������� ���� �� ���������� - ������ ��� �� �������
	{
		Create();
	}
}
//-----------------------------------------------------------------------------
QVariant ISConfig::GetValue(const QString &ParameterName)
{
	IS_ASSERT(Settings, "Config not initialized");
	IS_ASSERT(Settings->contains(ParameterName), QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	return Settings->value(ParameterName);
}
//-----------------------------------------------------------------------------
void ISConfig::SetValue(const QString &ParameterName, const QVariant &Value)
{
	IS_ASSERT(Settings->contains(ParameterName), QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	Settings->setValue(ParameterName, Value);
}
//-----------------------------------------------------------------------------
void ISConfig::Update()
{
	QSettings settings_template(PathConfigTemplate, QSettings::IniFormat); //������ ������ ����������������� �����
	QStringList Keys = settings_template.allKeys(); //���������� ��������� �������
	bool FlagChanged = false; //���� ���������
	
	for (const QString &Key : Keys) //������� ��������� �������
	{
		if (!Settings->contains(Key)) //���� ������ ��������� �� ���������� � ������� ���������������� ����� - ��������� ���
		{
			Settings->setValue(Key, settings_template.value(Key));
			FlagChanged = true;
		}
	}

	Keys = Settings->allKeys(); //���������� ��������� �������� ����������������� �����
	for (const QString &Key : Keys) //������� ��������� �������� ����������������� �����
	{
		if (!settings_template.contains(Key)) //���� � ������� ������ ��������� ��� - ������� ��� �� �������� ����������������� �����
		{
			Settings->remove(Key);
			FlagChanged = true;
		}
	}

	if (FlagChanged) //���� ��������� ���� �������� - ��������������
	{
		Settings->sync();
	}
}
//-----------------------------------------------------------------------------
void ISConfig::Create()
{
	QSettings settings_local(PathConfigTemplate, QSettings::IniFormat);
	for (const QString &Key : settings_local.allKeys())
	{
		Settings->setValue(Key, settings_local.value(Key));
	}
	Settings->sync();
}
//-----------------------------------------------------------------------------
