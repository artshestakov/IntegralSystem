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
	if (QFile::exists(PATH_CONFIG_FILE)) //Если конфигурационный файл существует - читаем его в память и проверяем необходимость обновления
	{
		Update();
	}
	else //Конфигурационный файл не существует - создаём его из шаблона
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
	QSettings settings_template(PathConfigTemplate, QSettings::IniFormat); //Читаем шаблон конфигурационного файла
	QStringList Keys = settings_template.allKeys(); //Запоминаем параметры шаблона
	bool FlagChanged = false; //Флаг изменения
	
	for (const QString &Key : Keys) //Обходим параметры шаблона
	{
		if (!Settings->contains(Key)) //Если такого параметра не существует в текущем конфигурационном файле - добавляем его
		{
			Settings->setValue(Key, settings_template.value(Key));
			FlagChanged = true;
		}
	}

	Keys = Settings->allKeys(); //Запоминаем параметры текущего конфигурационного файла
	for (const QString &Key : Keys) //Обходим параметры текущего конфигурационного файла
	{
		if (!settings_template.contains(Key)) //Если в шаблоне такого параметра нет - удаляем его из текущего конфигурационного файла
		{
			Settings->remove(Key);
			FlagChanged = true;
		}
	}

	if (FlagChanged) //Если параметры были изменены - синхронизируем
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
