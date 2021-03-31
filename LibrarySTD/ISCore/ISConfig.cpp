#include "ISConfig.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
    : ErrorString(STRING_NO_ERROR)
{
    //Структура шаблонов для конфигурационных файлов
    VectorTemplate = std::vector<ISConfigParameter>
    {
        //Серверный шаблон
        { CONFIG_TEMPLATE_SERVER, "TCPServer",	"Include",			CONFIG_TYPE_BOOL,	true,	"false",						0, 0 },
        { CONFIG_TEMPLATE_SERVER, "TCPServer",	"Port",				CONFIG_TYPE_INT,	true,	std::to_string(CARAT_TCP_PORT),	1, UINT16_MAX },
        { CONFIG_TEMPLATE_SERVER, "TCPServer",	"WorkerCount",		CONFIG_TYPE_INT,	true,	"1",							1, (int)std::thread::hardware_concurrency() * 2 },
        { CONFIG_TEMPLATE_SERVER, "Other",		"Configuration",	CONFIG_TYPE_STRING,	true,	std::string(),					0, 0 },

        //Клиентский шаблон
        { CONFIG_TEMPLATE_CLIENT, "Connection",		"Host",		CONFIG_TYPE_STRING,	true,	std::string(),					0, 0 },
        { CONFIG_TEMPLATE_CLIENT, "Connection",		"Port",		CONFIG_TYPE_INT,	true,	std::to_string(CARAT_TCP_PORT),	1, UINT16_MAX },
        { CONFIG_TEMPLATE_CLIENT, "RememberUser",	"Include",	CONFIG_TYPE_BOOL,	true,	"false",						0, 0 },
        { CONFIG_TEMPLATE_CLIENT, "RememberUser",	"Login",	CONFIG_TYPE_STRING,	true,	std::string(),					0, 0 }
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
    //Проверяем параметры на заполняемость
    for (const ISConfigParameter &ConfigParameter : VectorTemplate)
    {
        if (ConfigParameter.TemplateName != TemplateName)
        {
            continue;
        }
        std::string Value = GetValue(ConfigParameter.SectionName, ConfigParameter.Name);

        //Если параметр обязателен для заполнения и он пустой - ошибка
        if (ConfigParameter.NotNull && Value.empty())
        {
            ErrorString = ISAlgorithm::StringF("Parameter \"%s/%s\" is empty", ConfigParameter.SectionName.c_str(), ConfigParameter.Name.c_str());
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

        std::string Value = GetValue(ConfigParameter.SectionName, ConfigParameter.Name);
        if (ConfigParameter.Type == CONFIG_TYPE_INT) //Если текущий параметр - число
        {
            if (!ISAlgorithm::StringIsNumber(Value))
            {
                ErrorString = ISAlgorithm::StringF("Invalid parameter value %s/%s=%s", ConfigParameter.SectionName.c_str(), ConfigParameter.Name.c_str(), Value.c_str());
                return false;
            }
            int IntValue = std::stoi(Value);

            //Проверяем вхождение значения в диапазон
            if (IntValue < ConfigParameter.Minimum || IntValue > ConfigParameter.Maximum)
            {
                ErrorString = ISAlgorithm::StringF("Parameter %s/%s=%s out of range [%d;%d]", ConfigParameter.SectionName.c_str(), ConfigParameter.Name.c_str(), Value.c_str(), ConfigParameter.Minimum, ConfigParameter.Maximum);
                return false;
            }
        }
        else if (ConfigParameter.Type == CONFIG_TYPE_BOOL) //Если текущий параметр - логический
        {
            ISAlgorithm::StringToLower(Value);
            if (!(Value == "true" || Value == "false"))
            {
                ErrorString = ISAlgorithm::StringF("Invalid parameter value %s/%s=%s (allower \"true\" or \"false\"", ConfigParameter.SectionName.c_str(), ConfigParameter.Name.c_str(), Value.c_str());
                return false;
            }
        }
        else if (ConfigParameter.Type == CONFIG_TYPE_STRING)
        {
            //Для строк ничего не делаем
        }
        else //Неизвестный тип
        {
            ErrorString = ISAlgorithm::StringF("Invalid parameter type \"%s\" from %s/%s", ConfigParameter.Type.c_str(), ConfigParameter.SectionName.c_str(), ConfigParameter.Name.c_str());
            return false;
        }
    }
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
    return std::stoi(GetValue(SectionName, ParameterName));
}
//-----------------------------------------------------------------------------
unsigned short ISConfig::GetValueUShort(const std::string &SectionName, const std::string &ParameterName)
{
    return static_cast<unsigned short>(GetValueInt(SectionName, ParameterName));
}
//-----------------------------------------------------------------------------
bool ISConfig::GetValueBool(const std::string &SectionName, const std::string &ParameterName)
{
    //Получаем значение, приводим его к нижнему регистру и проверяем
    std::string Value = GetValue(SectionName, ParameterName);
    ISAlgorithm::StringToLower(Value);
    return Value == "true";
}
//-----------------------------------------------------------------------------
std::string ISConfig::GetValue(const std::string &SectionName, const std::string &ParameterName)
{
    std::string Value;
    CRITICAL_SECTION_LOCK(&CriticalSection);
    Value = SimpleINI.GetValue(SectionName.c_str(), ParameterName.c_str());
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
    return Value;
}
//-----------------------------------------------------------------------------
bool ISConfig::Update()
{
    if (SimpleINI.LoadFile(PathConfigFile.c_str()) != SI_OK)
    {
        return false;
    }

    bool NeedSave = false; //Флаг необходимости сохранения
    ISVectorString TempKeys; //Временный список всех ключей
    CSimpleIni::TNamesDepend Sections, Keys;
    SimpleINI.GetAllSections(Sections);
    for (const CSimpleIni::Entry &Section : Sections) //Обходим все секции
    {
        SimpleINI.GetAllKeys(Section.pItem, Keys);
        for (const CSimpleIni::Entry &Key : Keys) //Обходим все ключи
        {
            if (!ContainsKey(Section.pItem, Key.pItem)) //Если такого ключа в шаблоне нет - удаляем его из текущео файла
            {
                SimpleINI.Delete(Section.pItem, Key.pItem);
                NeedSave = true;
            }
            TempKeys.emplace_back(std::string(Section.pItem) + '/' + std::string(Key.pItem));
        }
    }

    //Теперь проверяем, не появилось ли новых параметров в шаблоне
    for (const ISConfigParameter &ConfigParameter : VectorTemplate) //Обходим параметры из шаблона
    {
        //Такого ключа в текущем конфигурационном файле нет - добавляем
        if (ConfigParameter.TemplateName == TemplateName &&
            !ISAlgorithm::VectorContains(TempKeys,
                ConfigParameter.SectionName + '/' + ConfigParameter.Name))
        {
            SimpleINI.SetValue(ConfigParameter.SectionName.c_str(), ConfigParameter.Name.c_str(), ConfigParameter.DefaultValue.c_str());
            NeedSave = true;
        }
    }

    if (NeedSave) //Требуется сохранение
    {
        if (SimpleINI.SaveFile(PathConfigFile.c_str()) != SI_OK)
        {
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::Create()
{
    for (const ISConfigParameter &ConfigParameter : VectorTemplate)
    {
        if (ConfigParameter.TemplateName != TemplateName)
        {
            continue;
        }
        SimpleINI.SetValue(ConfigParameter.SectionName.c_str(), ConfigParameter.Name.c_str(), ConfigParameter.DefaultValue.c_str());
    }
    return SimpleINI.SaveFile(PathConfigFile.c_str()) == SI_OK;
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
