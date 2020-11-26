#include "ISConfig.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: ErrorString(NO_ERROR_STRING),
	ErrorLine(0),
	ErrorColumn(0),
	Settings(nullptr)
{
	//Структура шаблонов для конфигурационных файлов
	VectorTemplate = std::vector<ISConfigParameter>
	{
		//Серверный шаблон
		{ CONFIG_TEMPLATE_SERVER, "Connection/Host",		QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Connection/Port",		QVariant::Int,		true,	QVariant(),				1, MAXUINT16 },
		{ CONFIG_TEMPLATE_SERVER, "Connection/Database",	QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Connection/Login",		QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Connection/Password",	QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Controller/Include",		QVariant::Bool,		true,	true,					0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Controller/Port",		QVariant::Int,		true,	CARAT_CONTROLLER_PORT,	1, MAXUINT16 },
		{ CONFIG_TEMPLATE_SERVER, "TCPServer/Include",		QVariant::Bool,		true,	false,					0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "TCPServer/Port",			QVariant::Int,		true,	CARAT_TCP_PORT,			1, MAXUINT16 },
		{ CONFIG_TEMPLATE_SERVER, "TCPServer/WorkerCount",	QVariant::Int,		true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Include",			QVariant::Bool,		true,	false,					0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Host",				QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Port",				QVariant::Int,		true,	QVariant(),				1, MAXUINT16 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Login",				QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Password",			QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/RecordDir",			QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Other/UpdateClientDir",	QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Other/Configuration",	QVariant::String,	true,	QVariant(),				0, 0 },

		//Клиентский шаблон
		{ CONFIG_TEMPLATE_CLIENT, "Connection/Host",		QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_CLIENT, "Connection/Port",		QVariant::Int,		true,	QVariant(),				1, MAXUINT16 },
		{ CONFIG_TEMPLATE_CLIENT, "Connection/Database",	QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_CLIENT, "RememberUser/Include",	QVariant::Bool,		true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_CLIENT, "RememberUser/Login",		QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_CLIENT, "Protocol/Include",		QVariant::Bool,		true,	false,					0, 0 },
		{ CONFIG_TEMPLATE_CLIENT, "Protocol/Port",			QVariant::Int,		true,	CARAT_TCP_PORT,			1, MAXUINT16 }
	};
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISConfig::~ISConfig()
{
	if (Settings)
	{
		delete Settings;
	}
	CRITICAL_SECTION_DESTROY(&CriticalSection);
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
QString ISConfig::GetConfigPath() const
{
	return PathConfigFile;
}
//-----------------------------------------------------------------------------
bool ISConfig::ReInitialize(const QString &template_name)
{
	//Удаляем указатель и инициализируем
	POINTER_DELETE(Settings);
	return Initialize(template_name);
}
//-----------------------------------------------------------------------------
bool ISConfig::Initialize(const QString &template_name)
{
	if (Settings) //Если указатель существует - значит файл был проинициализирован
	{
		ISLOGGER_W(__CLASS__, "Already initialized");
		return true;
	}
	TemplateName = template_name;

	PathConfigFile = QCoreApplication::applicationDirPath() + '/' + TemplateName + SYMBOL_POINT + EXTENSION_INI;
	Settings = new QSettings(PathConfigFile, QSettings::IniFormat);
	QSettings::Status SettingsStatus = Settings->status();
	bool Result = SettingsStatus == QSettings::NoError;
	if (Result)
	{
		//Если конфигурационный файл существует - читаем его в память и проверяем необходимость обновления, иначе создаём файл по шаблону
		Result = QFile::exists(PathConfigFile) ? Update() : Create();
	}
	else
	{
		switch (SettingsStatus)
		{
		case QSettings::NoError: break;
		case QSettings::AccessError: ErrorString = "access error with path " + PathConfigFile; break;
		case QSettings::FormatError: ErrorString = "format error"; break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
QVariant ISConfig::GetValue(const QString &ParameterName)
{
	QVariant Value;
	CRITICAL_SECTION_LOCK(&CriticalSection);
	bool Contains = Settings->contains(ParameterName);
	if (Contains)
	{
		Value = Settings->value(ParameterName);
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);

	if (!Contains)
	{
		ISLOGGER_W(__CLASS__, QString("Not found key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
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
		ISLOGGER_W(__CLASS__, QString("Not found key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
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
	bool FlagChanged = false; //Флаг изменения
	for (const QString &Key : Settings->allKeys()) //Проверяем каждый ключ на наличие в шаблоне
	{
		if (!ContainsKey(Key)) //Если такого ключа в шаблоне нет - удаляем его из текущео файла
		{
			Settings->remove(Key);
			FlagChanged = true;
		}
	}

	//Теперь проверяем, не появилось ли новых параметров в шаблоне
	for (const ISConfigParameter &ConfigParameter : VectorTemplate) //Обходим параметры из шаблона
	{
		if (ConfigParameter.TemplateName == TemplateName && !Settings->contains(ConfigParameter.Name)) //Такого ключа в текущем конфигурационном файле нет - добавляем
		{
			Settings->setValue(ConfigParameter.Name, ConfigParameter.DefaultValue.toString());
			FlagChanged = true;
		}
	}

	if (FlagChanged) //Если параметры были изменены - синхронизируем
	{
		SaveForce();
		if (Settings->status() != QSettings::NoError)
		{
			ErrorString = "error with update config file";
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::Create()
{
	//Обходим все параметры текущего шаблона и добавляем их в конфигурационный файл
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName == TemplateName)
		{
			Settings->setValue(ConfigParameter.Name, ConfigParameter.DefaultValue.toString());
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::ContainsKey(const QString &Key)
{
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName == TemplateName && ConfigParameter.Name == Key)
		{
			return true;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
