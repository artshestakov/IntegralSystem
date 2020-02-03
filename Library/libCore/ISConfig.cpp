#include "ISConfig.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISDebug.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: ErrorString("No error."),
	Settings(nullptr),
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
bool ISConfig::Initialize()
{
	bool Result = QFile::exists(PATH_CONFIG_FILE);
	if (Result) //���� ���������������� ���� ���������� - ������ ��� � ������ � ��������� ������������� ����������
	{
		Settings = new QSettings(PATH_CONFIG_FILE, QSettings::IniFormat);
		Result = Update();
	}
	else //���������������� ���� �� ���������� - ������ ��� �� �������
	{
		Result = Create();
		Settings = new QSettings(PATH_CONFIG_FILE, QSettings::IniFormat);
	}
	return Result;
}
//-----------------------------------------------------------------------------
QVariant ISConfig::GetValue(const QString &ParameterName)
{
	IS_ASSERT(Settings, "Config not initialized");
	IS_ASSERT(Settings->contains(ParameterName), QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	return Settings->value(ParameterName);
}
//-----------------------------------------------------------------------------
bool ISConfig::GetValueBool(const QString &ParameterName)
{
	return GetValue(ParameterName).toBool();
}
//-----------------------------------------------------------------------------
QString ISConfig::GetValueString(const QString &ParameterName)
{
	return GetValue(ParameterName).toString();
}
//-----------------------------------------------------------------------------
int ISConfig::GetValueInt(const QString &ParameterName)
{
	return GetValue(ParameterName).toInt();
}
//-----------------------------------------------------------------------------
void ISConfig::SetValue(const QString &ParameterName, const QVariant &Value)
{
	IS_ASSERT(Settings->contains(ParameterName), QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	Settings->setValue(ParameterName, Value);
}
//-----------------------------------------------------------------------------
void ISConfig::ClearValue(const QString &ParameterName)
{
	IS_ASSERT(Settings->contains(ParameterName), QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	Settings->setValue(ParameterName, QVariant());
}
//-----------------------------------------------------------------------------
bool ISConfig::Update()
{
	QSettings settings_local(PathConfigTemplate, QSettings::IniFormat);
	QStringList Keys = settings_local.allKeys();
	int CountAdded = 0; //���������� �����������
	for (const QString &Key : Keys)
	{
		if (!Settings->contains(Key)) //���� ����� ���� �� ���������� � ���������������� ����� - ��������� ���
		{
			Settings->setValue(Key, settings_local.value(Key));
			++CountAdded;
		}
	}

	if (CountAdded) //���� ��������� ���� ��������� - ��������������
	{
		Settings->sync();
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::Create()
{
	QFile File(PathConfigTemplate);
	bool Result = File.copy(PATH_CONFIG_FILE);
	if (!Result)
	{
		ErrorString = "Error create config file \"" + PATH_CONFIG_FILE + "\": " + File.errorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
