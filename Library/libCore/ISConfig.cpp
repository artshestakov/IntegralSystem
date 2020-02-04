#include "ISConfig.h"
#include "EXDefines.h"
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
QString ISConfig::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISConfig::Initialize()
{
	bool Result = QFile::exists(PathConfigTemplate);
	if (Result)
	{
		Result = !Settings;
		if (Result)
		{
			Settings = new QSettings(PATH_CONFIG_FILE, QSettings::IniFormat);
			QSettings::Status SettingsStatus = Settings->status();
			Result = SettingsStatus == QSettings::NoError;
			if (Result)
			{
				if (QFile::exists(PATH_CONFIG_FILE)) //���� ���������������� ���� ���������� - ������ ��� � ������ � ��������� ������������� ����������
				{
					Result = Update();
				}
				else //���������������� ���� �� ���������� - ������ ��� �� �������
				{
					Result = Create();
				}
			}
			else
			{
				switch (SettingsStatus)
				{
				case QSettings::AccessError: ErrorString = "access error with path " + PATH_CONFIG_FILE; break;
				case QSettings::FormatError: ErrorString = "format error"; break;
				}
			}
		}
		else
		{
			ErrorString = "config already initialized";
		}
	}
	else
	{
		ErrorString = "not found file template for config with path '" + PathConfigTemplate + "'";
	}
	return Result;
}
//-----------------------------------------------------------------------------
QVariant ISConfig::GetValue(const QString &ParameterName)
{
	QVariant Value;
	if (Settings->contains(ParameterName))
	{
		Value = Settings->value(ParameterName);
	}
	else
	{
		ISDebug::ShowWarningString(QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	}
	return Value;
}
//-----------------------------------------------------------------------------
void ISConfig::SetValue(const QString &ParameterName, const QVariant &Value)
{
	if (Settings->contains(ParameterName))
	{
		Settings->setValue(ParameterName, Value);
	}
	else
	{
		ISDebug::ShowWarningString(QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	}
}
//-----------------------------------------------------------------------------
bool ISConfig::Update()
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

	bool Result = true;
	if (FlagChanged) //���� ��������� ���� �������� - ��������������
	{
		Settings->sync();
		Result = Settings->status() == QSettings::NoError;
		if (!Result)
		{
			ErrorString = "error with update config file";
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISConfig::Create()
{
	QSettings settings_local(PathConfigTemplate, QSettings::IniFormat);
	for (const QString &Key : settings_local.allKeys())
	{
		Settings->setValue(Key, settings_local.value(Key));
	}
	Settings->sync();
	bool Result = Settings->status() == QSettings::NoError;
	if (!Result)
	{
		ErrorString = "error with create config file";
	}
	return Result;
}
//-----------------------------------------------------------------------------
