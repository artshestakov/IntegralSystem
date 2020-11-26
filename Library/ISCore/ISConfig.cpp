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
	Settings(nullptr),
	PathConfigTemplate(":Other/ConfigTemplate.xml")
{
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

	//Проверяем наличие файла-шаблона
	if (!QFile::exists(PathConfigTemplate))
	{
		ErrorString = "not found file template for config with path '" + PathConfigTemplate + "'";
		return false;
	}

	//Читаем файл-шаблон
	QFile FileTemplate(PathConfigTemplate);
	if (!FileTemplate.open(QIODevice::ReadOnly))
	{
		ErrorString = "not open file template config: " + FileTemplate.errorString();
		return false;
	}
	QString Content = FileTemplate.readAll();
	FileTemplate.close();

	//Парсим XML-шаблон
	QDomDocument XmlDocument;
	if (!XmlDocument.setContent(Content, &ErrorString, &ErrorLine, &ErrorColumn))
	{
		ErrorString += QString(" Line: %1. Column: %2").arg(ErrorLine).arg(ErrorColumn);
		return false;
	}
	QDomElement DomElement = XmlDocument.documentElement();
	DomNodeTemplate = DomElement.firstChildElement();

	//Ищем нужную секцию шаблона
	while (!DomNodeTemplate.isNull())
	{
		if (DomNodeTemplate.attributes().namedItem("Name").nodeValue() == TemplateName) //Нашли
		{
			//Спускаемся на уровень секций и выходим из цикла
			DomNodeTemplate = DomNodeTemplate.firstChild();
			break;
		}
		DomNodeTemplate = DomNodeTemplate.nextSibling(); //Не нашли - ищем дальше
	}

	//Проверяем, нашли ли нужный шаблон
	if (DomNodeTemplate.isNull())
	{
		ErrorString = "not found template section '" + TemplateName + "'";
		return false;
	}

	ISStringMap StringMap;
	if (!ReadXML(StringMap)) //При разборе XML-шаблона произошла ошибка
	{
		return false;
	}

	PathConfigFile = QCoreApplication::applicationDirPath() + '/' + TemplateName + SYMBOL_POINT + EXTENSION_INI;
	Settings = new QSettings(PathConfigFile, QSettings::IniFormat);
	QSettings::Status SettingsStatus = Settings->status();
	bool Result = SettingsStatus == QSettings::NoError;
	if (Result)
	{
		//Если конфигурационный файл существует - читаем его в память и проверяем необходимость обновления, иначе создаём файл по шаблону
		Result = QFile::exists(PathConfigFile) ? Update(StringMap) : Create(StringMap);
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
bool ISConfig::ReadXML(ISStringMap &StringMap)
{
	while (!DomNodeTemplate.isNull())
	{
		QString SectionName = DomNodeTemplate.attributes().namedItem("Name").nodeValue();
		if (SectionName.isEmpty()) //Если имя секции пустое - считаем за ошибку
		{
			ErrorString = QString("empty section name. Line: %1").arg(DomNodeTemplate.lineNumber());
			return false;
		}

		//Спускаемся к параметрам
		QDomNode DomNode = DomNodeTemplate.firstChild();
		while (!DomNode.isNull())
		{
			if (DomNode.isComment()) //Если параметр закоментирован - переходим к следующему
			{
				continue;
			}

			//Получаем имя параметра
			QString ParameterName = DomNode.attributes().namedItem("Name").nodeValue();
			if (ParameterName.isEmpty()) //Имя параметра пустое - считаем за ошибку
			{
				ErrorString = QString("empty parameter name. Line: %1").arg(DomNode.lineNumber());
				return false;
			}
			QString DefaultValue = DomNode.attributes().namedItem("DefaultValue").nodeValue();
			StringMap[SectionName + '/' + ParameterName] = DefaultValue;
			DomNode = DomNode.nextSibling(); //Переходим к следующему параметру
		}
		DomNodeTemplate = DomNodeTemplate.nextSibling(); //Переходим к следующей секции
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::Update(const ISStringMap &StringMap)
{
	bool FlagChanged = false; //Флаг изменения

	//Получаем текущие ключи и проверяем их наличие в шаблоне
	QStringList CurrentKeysList = Settings->allKeys();
	ISVectorString TemplateKeys = ISAlgorithm::ConvertMapToKeys(StringMap);
	for (const QString &Key : CurrentKeysList) //Обходим ключи
	{
		if (!ISAlgorithm::VectorContains(TemplateKeys, Key)) //Если такого ключа в шаблоне нет - удаляем его из текущео файла
		{
			Settings->remove(Key);
			FlagChanged = true;
		}
	}

	//Теперь проверяем, не появилось ли новых параметров в шаблоне
	for (const auto &MapItem : StringMap) //Обходим параметры из шаблона
	{
		if (!Settings->contains(MapItem.first)) //Такого ключа в текущем конфигурационном файле нет - добавляем
		{
			Settings->setValue(MapItem.first, MapItem.second);
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
bool ISConfig::Create(const ISStringMap &StringMap)
{
	for (const auto &MapItem : StringMap)
	{
		Settings->setValue(MapItem.first, MapItem.second);
	}
	SaveForce(); //Принудительно сохраняем

	if (Settings->status() != QSettings::NoError)
	{
		ErrorString = "error with create config file";
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
