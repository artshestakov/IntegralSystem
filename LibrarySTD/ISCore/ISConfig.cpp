#include "ISConfig.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include <unordered_map>
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: ErrorString(STRING_NO_ERROR)
{
	//��������� �������� ��� ���������������� ������
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
	//��������� �� ������� �������������
	/*if (!Settings)
	{
		ErrorString = "Not initialized";
		return false;
	}

	//��������� ��������� �� �������������
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName != TemplateName)
		{
			continue;
		}
		QVariant Value = GetValue(ConfigParameter.Name);

		//���� �������� ���������� ��� ����������, � ���� ��� �������� �� ��������� � �� ������ - ������
		if (ConfigParameter.NotNull && !ConfigParameter.DefaultValue.isValid() && Value.toString().isEmpty())
		{
			ErrorString = "Parameter \"" + ConfigParameter.Name + "\" is empty";
			return false;
		}
	}

	//��������� ������������ ����������
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName != TemplateName)
		{
			continue;
		}

		if (ConfigParameter.Type == QVariant::Int) //���� ������� �������� - �����
		{
			bool Ok = true;
			QVariant Value = GetValue(ConfigParameter.Name);
			int IntValue = Value.toInt(&Ok);
			if (!Ok) //�� ������� �������� �������� � �����
			{
				ErrorString = QString("Invalid paramter value %1=%2").arg(ConfigParameter.Name).arg(Value.toString());
				return false;
			}

			//��������� ��������� �������� � ��������
			if (IntValue < ConfigParameter.Minimum || IntValue > ConfigParameter.Maximum)
			{
				ErrorString = QString("Parameter %1=%2 out of range [%3;%4]").arg(ConfigParameter.Name).arg(IntValue).arg(ConfigParameter.Minimum).arg(ConfigParameter.Maximum);
				return false;
			}
		}
		else if (ConfigParameter.Type == QVariant::Bool) //���� ������� �������� - ����������
		{
			//��������� ������������ ��������
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
	//��������� ����
	std::ifstream File;
	File.open(PathConfigFile);
	if (!File.is_open())
	{
		ErrorString = "not open file \"" + PathConfigFile + "\": " + ISAlgorithm::GetLastErrorS();
		return false;
	}

	//������ ���� ���������
	std::string Line, CurrentSection;
	while (std::getline(File, Line))
	{
		if (Line.empty())
		{
			continue;
		}

		if (Line.front() == '[' && Line.back() == ']') //�������� ������
		{
			CurrentSection = Line.substr(1, Line.size() - 2);
		}
		else //��������
		{
			size_t Pos = Line.find('=');
			if (Pos != NPOS) //����� �����������
			{
				MapValues[CurrentSection + '/' + Line.substr(0, Pos)] =
					Pos == Line.size() - 1 ? std::string() : Line.substr(Pos + 1, Line.size() - Pos - 1);
			}
		}
	}
	File.close();

	//���� ���������� ���������
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
	//���� ������ ������
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

	//������� ������
	bool IsBeginSection = true;
	for (const auto &SectionItem : It->second)
	{
		//���� ������ ������ - ������� ������ �� ���������
		if (!IsBeginSection)
		{
			File << std::endl;
		}
		IsBeginSection = false;

		//���������� ��� ������ � ������� � ���������
		File << '[' << SectionItem.first << ']' << std::endl;
		for (const ISConfigParameter &ConfigParameter : SectionItem.second)
		{
			File << ConfigParameter.Name << '=' << ConfigParameter.DefaultValue << std::endl;
			MapValues.emplace(SectionItem.first + '/' + ConfigParameter.Name, ConfigParameter.DefaultValue);
		}
	}
	File.close();

	//������� ��� ��������� �������� ������� � ��������� �� � ���������������� ����
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
	//���� ����������� ������ - ����������
	size_t Pos = Key.find('/');
	if (Pos != NPOS)
	{
		//����������� ��� ������ � ���������
		std::string SectionName = Key.substr(0, Pos),
			ParameterName = Key.substr(Pos + 1, Key.size() - Pos - 1);

		//������� ������
		for (const auto &MapItem : MapTemplate.find(TemplateName)->second)
		{
			if (MapItem.first == SectionName) //������ ������� - ������� ���������
			{
				for (const ISConfigParameter &ConfigParameter : MapItem.second)
				{
					if (ConfigParameter.Name == ParameterName) //����� �������� - �������
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
