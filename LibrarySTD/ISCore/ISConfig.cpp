#include "ISConfig.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISAssert.h"
#include "SimpleIni.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: ErrorString(STRING_NO_ERROR)
{
	//��������� �������� ��� ���������������� ������
	VectorTemplate = std::vector<ISConfigParameter>
	{
		//��������� ������
		{ CONFIG_TEMPLATE_SERVER, "TCPServer",	"Include",			"Bool",		true,	"false",						0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "TCPServer",	"Port",				"Int",		true,	std::to_string(CARAT_TCP_PORT),	1, UINT16_MAX },
		{ CONFIG_TEMPLATE_SERVER, "TCPServer",	"WorkerCount",		"Int",		true,	"1",							1, (int)std::thread::hardware_concurrency() * 2 },
		{ CONFIG_TEMPLATE_SERVER, "Other",		"Configuration",	"String",	true,	std::string(),					0, 0 },

		//���������� ������
		{ CONFIG_TEMPLATE_CLIENT, "Connection",		"Host",		"String",	true,	std::string(),					0, 0 },
		{ CONFIG_TEMPLATE_CLIENT, "Connection",		"Port",		"Int",		true,	std::to_string(CARAT_TCP_PORT),	1, UINT16_MAX },
		{ CONFIG_TEMPLATE_CLIENT, "RememberUser",	"Include",	"Bool",		true,	"false",						0, 0 },
		{ CONFIG_TEMPLATE_CLIENT, "RememberUser",	"Login",	"String",	true,	std::string(),					0, 0 }
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
	IS_UNUSED(SectionName);
	IS_UNUSED(ParameterName);
	std::string Value;
	/*bool Contains = false;
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
	IS_ASSERT(Contains, "Not found config parameter: " + SectionName + '/' + ParameterName);*/
	return Value;
}
//-----------------------------------------------------------------------------
bool ISConfig::Update()
{
	CSimpleIni SimpleIni;
	if (SimpleIni.LoadFile(PathConfigFile.c_str()) != SI_OK)
	{
		return false;
	}

	bool NeedSave = false; //���� ������������� ����������
	ISVectorString TempKeys; //��������� ������ ���� ������
	CSimpleIni::TNamesDepend Sections, Keys;
	SimpleIni.GetAllSections(Sections);
	for (const CSimpleIni::Entry &Section : Sections) //������� ��� ������
	{
		SimpleIni.GetAllKeys(Section.pItem, Keys);
		for (const CSimpleIni::Entry &Key : Keys) //������� ��� �����
		{
			if (!ContainsKey(Section.pItem, Key.pItem)) //���� ������ ����� � ������� ��� - ������� ��� �� ������� �����
			{
				SimpleIni.Delete(Section.pItem, Key.pItem);
				NeedSave = true;
			}
			TempKeys.emplace_back(std::string(Section.pItem) + '/' + std::string(Key.pItem));
		}
	}

	//������ ���������, �� ��������� �� ����� ���������� � �������
	for (const ISConfigParameter &ConfigParameter : VectorTemplate) //������� ��������� �� �������
	{
		//������ ����� � ������� ���������������� ����� ��� - ���������
		if (ConfigParameter.TemplateName == TemplateName &&
			!ISAlgorithm::VectorContains(TempKeys,
				ConfigParameter.SectionName + '/' + ConfigParameter.Name))
		{
			SimpleIni.SetValue(ConfigParameter.SectionName.c_str(), ConfigParameter.Name.c_str(), ConfigParameter.DefaultValue.c_str());
			NeedSave = true;
		}
	}

	if (NeedSave) //��������� ����������
	{
		if (SimpleIni.SaveFile(PathConfigFile.c_str()) != SI_OK)
		{
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::Create()
{
	CSimpleIni SimpleIni;
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName != TemplateName)
		{
			continue;
		}
		SimpleIni.SetValue(ConfigParameter.SectionName.c_str(), ConfigParameter.Name.c_str(), ConfigParameter.DefaultValue.c_str());
	}
	return SimpleIni.SaveFile(PathConfigFile.c_str()) == SI_OK;
}
//-----------------------------------------------------------------------------
bool ISConfig::ContainsKey(const std::string &SectionName, const std::string &ParameterName)
{
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName == TemplateName &&
			ConfigParameter.SectionName == SectionName &&
			ConfigParameter.Name == ParameterName)
		{
			return true;
		}

	}
	return false;
}
//-----------------------------------------------------------------------------
