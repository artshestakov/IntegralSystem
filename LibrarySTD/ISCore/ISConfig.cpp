#include "ISConfig.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include <unordered_map>
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: ErrorString(STRING_NO_ERROR)
{
	//Структура шаблонов для конфигурационных файлов
	MapTemplate = std::map<std::string, std::map<std::string, std::vector<ISConfigParameter>>>
	{
		{
			CONFIG_TEMPLATE_SERVER,
			{
				{
					"TCPServer", std::vector<ISConfigParameter>
					{
						{ "Include",		"Bool",		true,	"false",						0, 0											},
						{ "Port",			"Int",		true,	std::to_string(CARAT_TCP_PORT),	1, UINT16_MAX									},
						{ "WorkerCount",	"Int",		true,	"1",							1, (int)std::thread::hardware_concurrency() * 2	}
					}
				},
				{
					"Other", std::vector<ISConfigParameter>
					{
						{ "Configuration",	"String",	true,	std::string(),					0, 0 }
					}
				}
			}
		},
		{
			CONFIG_TEMPLATE_CLIENT,
			{

				{
					"Connection", std::vector<ISConfigParameter>
					{
						{ "Host", "String",	true, std::string(),					0, 0 },
						{ "Port", "Int",	true, std::to_string(CARAT_TCP_PORT),	1, UINT16_MAX },
					}
				},
				{
					"RememberUser", std::vector<ISConfigParameter>
					{
						{ "Include",	"Bool",		true,	"false",		0, 0 },
						{ "Login",		"String",	true,	std::string(),	0, 0 }
					}
				}
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
bool ISConfig::Initialize(const std::string &template_name)
{
	TemplateName = template_name;
	PathConfigFile = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + TemplateName + ".ini";
	return ISAlgorithm::FileExist(PathConfigFile) ? Update() : Create();
}
//-----------------------------------------------------------------------------
std::string ISConfig::GetValue(const std::string &ParameterName)
{
	IS_UNUSED(ParameterName);
	std::string Value;
	/*CRITICAL_SECTION_LOCK(&CriticalSection);
	bool Contains = Settings->contains(ParameterName);
	if (Contains)
	{
		Value = Settings->value(ParameterName);
		if (Value.toString().isEmpty())
		{
			Value = GetDefaultValue(ParameterName);
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	IS_ASSERT(Contains, QString("Not found key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));*/
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
				MapValues[CurrentSection + '/' + Line.substr(0, Pos)] =
					Pos == Line.size() - 1 ? std::string() : Line.substr(Pos + 1, Line.size() - Pos - 1);
			}
		}
	}
	File.close();

	//Ищем устаревшие параметры
	for (const auto &ParameterItem : MapValues)
	{
		if (!ContainsKey(ParameterItem.first))
		{

		}
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::Create()
{
	//Ищем нужный шаблон
	auto It = MapTemplate.find(TemplateName);
	if (It == MapTemplate.end())
	{
		ErrorString = "not found template \"" + TemplateName + "\"";
		return false;
	}

	std::ofstream File;
	File.open(PathConfigFile, std::ios::out);
	if (!File.is_open())
	{
		ErrorString = "not open file \"" + PathConfigFile + "\": " + ISAlgorithm::GetLastErrorS();
		return false;
	}

	//Обходим секции
	bool IsBeginSection = true;
	for (const auto &SectionItem : It->second)
	{
		//Если секция первая - перевод строки не вставляем
		if (!IsBeginSection)
		{
			File << std::endl;
		}
		IsBeginSection = false;

		//Записываем имя секции и обходим её параметры
		File << '[' << SectionItem.first << ']' << std::endl;
		for (const ISConfigParameter &ConfigParameter : SectionItem.second)
		{
			File << ConfigParameter.Name << '=' << ConfigParameter.DefaultValue << std::endl;
			MapValues.emplace(SectionItem.first + '/' + ConfigParameter.Name, ConfigParameter.DefaultValue);
		}
	}
	File.close();

	//Обходим все параметры текущего шаблона и добавляем их в конфигурационный файл
	/*for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName == TemplateName)
		{
			Settings->setValue(ConfigParameter.Name, ConfigParameter.DefaultValue.toString());
		}
	}*/
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::ContainsKey(const std::string &Key)
{
	//Если разделитель найден - продолжаем
	size_t Pos = Key.find('/');
	if (Pos != NPOS)
	{
		//Вытаскиваем имя секции и параметра
		std::string SectionName = Key.substr(0, Pos),
			ParameterName = Key.substr(Pos + 1, Key.size() - Pos - 1);

		//Обходим секции
		for (const auto &MapItem : MapTemplate.find(TemplateName)->second)
		{
			if (MapItem.first == SectionName) //Секция нашлась - обходим параметры
			{
				for (const ISConfigParameter &ConfigParameter : MapItem.second)
				{
					if (ConfigParameter.Name == ParameterName) //Нашли параметр - выходим
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
std::string ISConfig::GetDefaultValue(const std::string &Key) const
{
	IS_UNUSED(Key);
	/*for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName == TemplateName && ConfigParameter.Name == Key)
		{
			return ConfigParameter.DefaultValue;
		}
	}*/
	return std::string();
}
//-----------------------------------------------------------------------------
