#include "ISConfig.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: ErrorString(STRING_NO_ERROR)
{
	//Шаблоны
	TemplateServer = std::map<std::string, std::map<std::string, ISConfigParameter>>
	{
		{ "TCPServer",
			{
				{ "Include", { "Bool", true, "false", std::string(), 0, 0 } },
				{ "Port", { "Int", true, std::to_string(CARAT_TCP_PORT), std::string(), 1, UINT16_MAX } },
				{ "WorkerCount", { "Int", true, "1", std::string(), 1, (int)std::thread::hardware_concurrency() * 2 } }
			}
		},
		{ "Other",
			{
				{ "Configuration", { "String", true, std::string(), std::string(), 0, 0 } }
			}
		}
	};
	TemplateClient = std::map<std::string, std::map<std::string, ISConfigParameter>>
	{
		{ "Connection",
			{
				{ "Host", { "String", true, std::string(), std::string(), 0, 0 } },
				{ "Port", { "Int", true, std::to_string(CARAT_TCP_PORT), std::string(), 1, UINT16_MAX } }
			}
		},
		{ "RememberUser",
			{
				{ "Include", { "Bool", true, "false", std::string(), 0, 0 } },
				{ "Login", { "String", true, std::string(), std::string(), 0, 0 } }
			}
		}
	};
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISConfig::~ISConfig()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISConfig& ISConfig::Instance()
{
	static ISConfig Config;
	return Config;
}
//-----------------------------------------------------------------------------
std::string ISConfig::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISConfig::IsValid()
{
	//Проверяем на наличие инициализации
	/*if (!Settings)
	{
		ErrorString = "Not initialized";
		return false;
	}

	//Проверяем параметры на заполняемость
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName != TemplateName)
		{
			continue;
		}
		QVariant Value = GetValue(ConfigParameter.Name);

		//Если параметр обязателен для заполнения, у него нет значения по умолчанию и он пустой - ошибка
		if (ConfigParameter.NotNull && !ConfigParameter.DefaultValue.isValid() && Value.toString().isEmpty())
		{
			ErrorString = "Parameter \"" + ConfigParameter.Name + "\" is empty";
			return false;
		}
	}

	//Проверяем корректность параметров
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName != TemplateName)
		{
			continue;
		}

		if (ConfigParameter.Type == QVariant::Int) //Если текущий параметр - число
		{
			bool Ok = true;
			QVariant Value = GetValue(ConfigParameter.Name);
			int IntValue = Value.toInt(&Ok);
			if (!Ok) //Не удалось привести значение к числу
			{
				ErrorString = QString("Invalid paramter value %1=%2").arg(ConfigParameter.Name).arg(Value.toString());
				return false;
			}

			//Проверяем вхождение значения в диапазон
			if (IntValue < ConfigParameter.Minimum || IntValue > ConfigParameter.Maximum)
			{
				ErrorString = QString("Parameter %1=%2 out of range [%3;%4]").arg(ConfigParameter.Name).arg(IntValue).arg(ConfigParameter.Minimum).arg(ConfigParameter.Maximum);
				return false;
			}
		}
		else if (ConfigParameter.Type == QVariant::Bool) //Если текущий параметр - логический
		{
			//Проверяем корректность значений
			QString StringValue = GetValue(ConfigParameter.Name).toString();
			if (!(StringValue.toLower() == "true" || StringValue.toLower() == "false"))
			{
				ErrorString = QString("Invalid paramter value %1=%2 (allowed \"true\" or \"false\")").arg(ConfigParameter.Name).arg(StringValue);
				return false;
			}
		}
	}*/
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::Initialize(ISNamespace::ConfigType Type)
{
	std::string FileName;
	switch (Type)
	{
	case ISNamespace::ConfigType::Server: FileName = "Server"; break;
	case ISNamespace::ConfigType::Client: FileName = "Client"; break;
	default:
		ErrorString = "unknown config type";
		return false;
	}
	MapConfig = GetTemplate(Type);
	PathConfigFile = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + FileName + ".ini";
	return ISAlgorithm::FileExist(PathConfigFile) ? Update() : Create();
}
//-----------------------------------------------------------------------------
std::string ISConfig::GetValueString(const std::string &SectionName, const std::string &ParameterName)
{
	return GetValue(SectionName, ParameterName);
}
//-----------------------------------------------------------------------------
int ISConfig::GetValueInt(const std::string &SectionName, const std::string &ParameterName)
{
	std::string Value = GetValue(SectionName, ParameterName);
	int IntValue = 0;
	try
	{
		IntValue = std::stoi(Value);
	}
	catch (const std::exception &e)
	{
		IS_ASSERT(false, e.what());
	}
	return IntValue;
}
//-----------------------------------------------------------------------------
bool ISConfig::GetValueBool(const std::string &SectionName, const std::string &ParameterName)
{
	return GetValue(SectionName, ParameterName) == "true";
}
//-----------------------------------------------------------------------------
std::string ISConfig::GetValue(const std::string &SectionName, const std::string &ParameterName)
{
	std::string Value;
	bool Contains = false;
	CRITICAL_SECTION_LOCK(&CriticalSection);
	auto ItSection = MapConfig.find(SectionName);
	if (ItSection != MapConfig.end())
	{
		auto ItParameter = ItSection->second.find(ParameterName);
		if (ItParameter != ItSection->second.end())
		{
			Value = ItParameter->second.Value;
			Contains = true;
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	IS_ASSERT(Contains, "Not found config parameter: " + SectionName + '/' + ParameterName);
	return Value;
}
//-----------------------------------------------------------------------------
bool ISConfig::Update()
{
	//Открываем файл
	std::ifstream File;
	File.open(PathConfigFile);
	if (!File.is_open())
	{
		ErrorString = "not open file \"" + PathConfigFile + "\": " + ISAlgorithm::GetLastErrorS();
		return false;
	}

	//Читаем файл построчно
	std::string Line, CurrentSection;
	while (std::getline(File, Line))
	{
		if (Line.empty())
		{
			continue;
		}

		if (Line.front() == '[' && Line.back() == ']') //Попалась секция
		{
			CurrentSection = Line.substr(1, Line.size() - 2);
		}
		else //Параметр
		{
			size_t Pos = Line.find('=');
			if (Pos != NPOS) //Нашли разделитель
			{
				std::string Name = Line.substr(0, Pos),
					Value = Line.substr(Pos + 1, Line.size() - Pos - 1);
				MapConfig[CurrentSection][Name].Value = Value;
			}
		}
	}
	File.close();
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::Create()
{
	std::ofstream File;
	File.open(PathConfigFile, std::ios::out);
	if (!File.is_open())
	{
		ErrorString = "not open file \"" + PathConfigFile + "\": " + ISAlgorithm::GetLastErrorS();
		return false;
	}

	//Обходим секции
	bool IsBeginSection = true;
	for (const auto &SectionItem : MapConfig)
	{
		//Если секция первая - перевод строки не вставляем
		if (!IsBeginSection)
		{
			File << std::endl;
		}
		IsBeginSection = false;

		//Записываем имя секции и обходим её параметры
		File << '[' << SectionItem.first << ']' << std::endl;
		for (const auto &ParameterItem : SectionItem.second)
		{
			File << ParameterItem.first << '=' << ParameterItem.second.DefaultValue << std::endl;
			TemplateServer[SectionItem.first][ParameterItem.first].Value = ParameterItem.second.DefaultValue;
		}
	}
	File.close();
	return true;
}
//-----------------------------------------------------------------------------
ISConfig::TemplateMap ISConfig::GetTemplate(ISNamespace::ConfigType Type) const
{
	switch (Type)
	{
	case ISNamespace::ConfigType::Server: return TemplateServer; break;
	case ISNamespace::ConfigType::Client: return TemplateClient; break;
	}
	return ISConfig::TemplateMap();
}
//-----------------------------------------------------------------------------
