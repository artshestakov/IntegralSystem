#include "ISConfig.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: ErrorString(NO_ERROR_STRING),
	Settings(nullptr)
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
ISConfig& ISConfig::Instance()
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
bool ISConfig::Initialize(const QString &TemplateName)
{
	if (Settings)
	{
		return true;
	}

	PathConfigTemplate = ":ConfigTemplate/" + TemplateName + SYMBOL_POINT + EXTENSION_INI;
	bool Result = QFile::exists(PathConfigTemplate);
	if (Result)
	{
		Settings = new QSettings(ISDefines::Core::PATH_CONFIG_FILE, QSettings::IniFormat);
		QSettings::Status SettingsStatus = Settings->status();
		Result = SettingsStatus == QSettings::NoError;
		if (Result)
		{
			if (QFile::exists(ISDefines::Core::PATH_CONFIG_FILE)) //���� ���������������� ���� ���������� - ������ ��� � ������ � ��������� ������������� ����������
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
            case QSettings::NoError: break;
			case QSettings::AccessError: ErrorString = "access error with path " + ISDefines::Core::PATH_CONFIG_FILE; break;
			case QSettings::FormatError: ErrorString = "format error"; break;
			}
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
		ISLOGGER_W(QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
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
		ISLOGGER_W(QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	}
}
//-----------------------------------------------------------------------------
void ISConfig::SaveForce()
{
	if (Settings)
	{
		Settings->sync();
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
