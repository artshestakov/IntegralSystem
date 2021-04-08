#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISMetaData::ISMetaData()
    : ErrorString(STRING_NO_ERROR),
    Initialized(false)
{
    CRITICAL_SECTION_INIT(&CS);
}
//-----------------------------------------------------------------------------
ISMetaData::~ISMetaData()
{
    CRITICAL_SECTION_DESTROY(&CS);
}
//-----------------------------------------------------------------------------
ISMetaData& ISMetaData::Instance()
{
    static ISMetaData MetaData;
    return MetaData;
}
//-----------------------------------------------------------------------------
std::string ISMetaData::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISMetaData::Init(const std::string &configuration_name, bool XSR, bool XSF)
{
    IS_UNUSED(configuration_name);
    IS_UNUSED(XSR);
    IS_UNUSED(XSF);

    //Если мета-данные уже инициализированны - выходим
    if (Initialized)
    {
        return true;
    }

    if (configuration_name.empty())
    {
        ErrorString = "Configuration name is empty";
    }
    ConfigurationName = configuration_name;

    if (!InitXSN())
    {
        return false;
    }

    if (XSR && !InitXSR())
    {
        return false;
    }

    if (XSF && !InitXSF())
    {
        return false;
    }

    Initialized = true;
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSN()
{
    //Открываем файл
    std::ifstream File("G:\\Github\\IntegralSystem\\Resources\\Scheme\\Asterisk.xsn");
    if (!File.is_open()) //Не удалось открыть файл
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }

    //Вычисляем размер
    File.seekg(0, std::ios::end);
    std::streampos StreamPos = File.tellg();
    File.seekg(0);

    //Читаем файл
    size_t Size = (size_t)StreamPos.seekpos();
    std::string Content(Size, CHAR_NULL_TERM);
    File.read(&Content[0], (long long)Size);
    File.close();
    return InitXSN(Content, Size, "Asterisk.xsn");
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSN(const std::string &Content, size_t Size, const std::string &FileName)
{
    //Парсим содержимое файла
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError XmlError = XmlDocument.Parse(Content.c_str(), Size);
    if (XmlError != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = XmlDocument.ErrorStr();
        return false;
    }

    //Проверим имя главного тега
    tinyxml2::XMLElement *XmlElement = XmlDocument.FirstChildElement();
    const char *TagName = XmlElement->Name();
    if (strcmp(TagName, "XSN") != 0)
    {
        ErrorString = ISAlgorithm::StringF("Invalid tag name. \"%s\" in XSN file \"%s\"", TagName, FileName.c_str());
        return false;
    }

    //Проверим наличие атрибута Name в главном теге
    const tinyxml2::XMLAttribute *XmlAttribute = XmlElement->FindAttribute("Name");
    if (!XmlAttribute)
    {
        ErrorString = ISAlgorithm::StringF("Not found attribute \"Name\" in a XSN file \"%s\"", FileName.c_str());
        return false;
    }
    CurrentXSN = XmlAttribute->Value();

    //Перебираем таблицы и парсим каждую
    while ((XmlElement = XmlElement->FirstChildElement()) != nullptr)
    {
        if (!InitXSNTable(XmlElement))
        {
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSNTable(tinyxml2::XMLElement *XmlElement)
{
    //Проверяем наличие атрибутов
    const tinyxml2::XMLAttribute *XmlAttribute = XmlElement->FirstAttribute();
    if (!XmlAttribute)
    {
        ErrorString = ISAlgorithm::StringF("Empty attributes. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Получаем имя таблицы
    const char *TableName = XmlElement->Attribute("Name");
    if (!TableName)
    {
        ErrorString = ISAlgorithm::StringF("Empty TableName in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Получаем идентификатор таблицы
    const char *UID = XmlElement->Attribute("UID");
    if (!UID)
    {
        ErrorString = ISAlgorithm::StringF("Empty UID in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Получаем псевдоним
    const char *Alias = XmlElement->Attribute("Alias");
    if (!Alias)
    {
        ErrorString = ISAlgorithm::StringF("Empty alias in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Получаем локальное имя
    const char *LocalName = XmlElement->Attribute("LocalName");
    if (!LocalName)
    {
        ErrorString = ISAlgorithm::StringF("Empty local name in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Получаем локальное имя в множественном числе
    const char *LocalListName = XmlElement->Attribute("LocalListName");
    if (!LocalListName)
    {
        ErrorString = ISAlgorithm::StringF("Empty local list name in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    PMetaTable *MetaTable = new PMetaTable();
    MetaTable->TableName = TableName;
    MetaTable->UID = UID;
    MetaTable->Alias = Alias;
    MetaTable->LocalName = LocalName;
    MetaTable->LocalListName = LocalListName;

    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSNTableSystemFields(PMetaTable *MetaTable)
{
    IS_UNUSED(MetaTable);
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSNTableFields(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement)
{
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSR()
{
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSF()
{
    return true;
}
//-----------------------------------------------------------------------------
