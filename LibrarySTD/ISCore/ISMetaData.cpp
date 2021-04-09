#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISAssert.h"
#include "ISResourcer.h"
//-----------------------------------------------------------------------------
ISMetaData::ISMetaData()
    : ErrorString(STRING_NO_ERROR),
    Initialized(false),
    CurrentXSN(nullptr),
    TypesCount(0)
{
    VectorFilesXSN.emplace_back("Scheme/Asterisk.xsn");
    VectorFilesXSN.emplace_back("Scheme/Settings.xsn");
    VectorFilesXSN.emplace_back("Scheme/SettingsDatabase.xsn");
    VectorFilesXSN.emplace_back("Scheme/System.xsn");
    VectorFilesXSN.emplace_back("Scheme/Task.xsn");
    VectorFilesXSN.emplace_back("Scheme/User.xsn");

    VectorTypes.emplace_back(ISMetaType("Unknown", ISNamespace::FieldType::Unknown, std::string(), std::string(), std::string(), false));
    VectorTypes.emplace_back(ISMetaType("ID", ISNamespace::FieldType::ID, "BIGINT", "ISIntegerEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("Int", ISNamespace::FieldType::Int, "INTEGER", "ISIntegerEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("IntArray", ISNamespace::FieldType::IntArray, "INTEGER[]", "ISIntegerEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("BigInt", ISNamespace::FieldType::BigInt, "BIGINT", "ISIntegerEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("String", ISNamespace::FieldType::String, "CHARACTER VARYING", "ISLineEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Text", ISNamespace::FieldType::Text, "TEXT", "ISTextEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("UID", ISNamespace::FieldType::UID, "UUID", "ISUuidEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Bool", ISNamespace::FieldType::Bool, "BOOLEAN", "ISCheckEdit", "ISComboSearchBase", true));
    VectorTypes.emplace_back(ISMetaType("Double", ISNamespace::FieldType::Double, "NUMERIC", "ISDoubleEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("Money", ISNamespace::FieldType::Money, "NUMERIC", "ISMoneyEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("ByteArray", ISNamespace::FieldType::ByteArray, "BYTEA", "ISLineEdit", std::string(), false));
    VectorTypes.emplace_back(ISMetaType("Date", ISNamespace::FieldType::Date, "DATE", "ISDateEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("Time", ISNamespace::FieldType::Time, "TIME WITHOUT TIME ZONE", "ISTimeEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("DateTime", ISNamespace::FieldType::DateTime, "TIMESTAMP WITHOUT TIME ZONE", "ISDateTimeEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("Inn", ISNamespace::FieldType::Inn, "CHARACTER VARYING", "ISINNEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Kpp", ISNamespace::FieldType::Kpp, "CHARACTER VARYING", "ISKPPEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Ogrn", ISNamespace::FieldType::Ogrn, "CHARACTER VARYING", "ISOGRNEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Okpo", ISNamespace::FieldType::Okpo, "CHARACTER VARYING", "ISOKPOEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Bik", ISNamespace::FieldType::Bik, "CHARACTER VARYING", "ISBIKEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Vin", ISNamespace::FieldType::Vin, "CHARACTER VARYING", "ISVINEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Year", ISNamespace::FieldType::Year, "INTEGER", "ISYearEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("Phone", ISNamespace::FieldType::Phone, "CHARACTER VARYING", "ISPhoneEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Image", ISNamespace::FieldType::Image, "BYTEA", "ISImageEdit", std::string(), false));
    VectorTypes.emplace_back(ISMetaType("Color", ISNamespace::FieldType::Color, "CHARACTER VARYING", "ISColorEdit", std::string(), false));
    VectorTypes.emplace_back(ISMetaType("EMail", ISNamespace::FieldType::EMail, "CHARACTER VARYING", "ISEMailEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("Volume", ISNamespace::FieldType::Volume, "INTEGER", "ISVolumeEdit", std::string(), false));
    VectorTypes.emplace_back(ISMetaType("Birthday", ISNamespace::FieldType::Birthday, "DATE", "ISBirthdayEdit", "ISComboSearchNumber", true));
    VectorTypes.emplace_back(ISMetaType("Seconds", ISNamespace::FieldType::Seconds, "INTEGER", "ISIntegerEdit", "ISComboSearchNumber", false));
    VectorTypes.emplace_back(ISMetaType("Month", ISNamespace::FieldType::Month, "INTEGER", "ISMonthEdit", "ISComboSearchNumber", false));
    VectorTypes.emplace_back(ISMetaType("Url", ISNamespace::FieldType::Url, "CHARACTER VARYING", "ISUrlEdit", "ISComboSearchString", true));
    VectorTypes.emplace_back(ISMetaType("File", ISNamespace::FieldType::File, "CHARACTER VARYING", "ISFileEdit", std::string(), false));
    VectorTypes.emplace_back(ISMetaType("TaskImportant", ISNamespace::FieldType::TaskImportant, "BOOLEAN", "ISTaskImportantEdit", "ISComboSearchBase", true));
    VectorTypes.emplace_back(ISMetaType("ProtocolDT", ISNamespace::FieldType::ProtocolDT, "TIMESTAMP WITHOUT TIME ZONE", "ISDateTimeEdit", "ISComboSearchNumber", true));
    TypesCount = VectorTypes.size();

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
ISNamespace::FieldType ISMetaData::GetType(const std::string &type)
{
    for (size_t i = 0; i < TypesCount; ++i)
    {
        if (VectorTypes[i].TypeName == type)
        {
            return VectorTypes[i].TypeField;
        }
    }
    IS_ASSERT(false, ISAlgorithm::StringF("Not found type by name \"%s\"", type.c_str()));
    return ISNamespace::FieldType::Unknown;
}
//-----------------------------------------------------------------------------
PMetaTable* ISMetaData::GetTable(const std::string &TableName)
{
    PMetaTable *Table = nullptr;
    CRITICAL_SECTION_LOCK(&CS);
    for (PMetaTable *MetaTable : Tables)
    {
        if (MetaTable->Name == TableName)
        {
            Table = MetaTable;
            break;
        }
    }
    CRITICAL_SECTION_UNLOCK(&CS);
    return Table;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSN()
{
    //Читаем файл ресурсов
    ISResourcer Resourcer;
    if (!Resourcer.LoadFile(ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Resources.bin"))
    {
        ErrorString = ISAlgorithm::StringF("Not read resource file: %s", Resourcer.GetErrorString().c_str());
        return false;
    }

    //Получаем содержимое шаблона
    unsigned long TemplateSize = 0;
    const char *TemplateContent = Resourcer.GetFile("Other/ClassTemplateFields.xml", TemplateSize);

    //Парсим содержимое шаблона
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError XmlError = XmlDocument.Parse(TemplateContent, TemplateSize);
    if (XmlError != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = ISAlgorithm::StringF("Not parse template system fields: %s", XmlDocument.ErrorStr());
        return false;
    }

    //Получаем главный тег шаблона
    tinyxml2::XMLElement *XmlElementTemplateXNS = XmlDocument.FirstChildElement();
    if (!XmlElementTemplateXNS)
    {
        ErrorString = ISAlgorithm::StringF("Not exist main tag in template system fields");
        return false;
    }

    for (const std::string &FileName : VectorFilesXSN)
    {
        unsigned long ContentSize = 0;
        const char *Content = Resourcer.GetFile(FileName, ContentSize);
        if (!InitXSN(Content, (size_t)ContentSize, FileName, XmlElementTemplateXNS))
        {
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSN(const std::string &Content, size_t Size, const std::string &FileName, tinyxml2::XMLElement *XmlElementTemplateXNS)
{
    //Парсим содержимое файла
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError XmlError = XmlDocument.Parse(Content.c_str(), Size);
    if (XmlError != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = ISAlgorithm::StringF("Not parse file \"%s\": %s", FileName.c_str(), XmlDocument.ErrorStr());
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

    //Переходим к таблицам и перебираем каждую
    XmlElement = XmlElement->FirstChildElement();
    while (XmlElement)
    {
        if (!InitXSNTable(XmlElement, XmlElementTemplateXNS))
        {
            return false;
        }
        XmlElement = XmlElement->NextSiblingElement();
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitXSNTable(tinyxml2::XMLElement *XmlElement, tinyxml2::XMLElement *XmlElementTemplateXNS)
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
    if (!TableName || strlen(TableName) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty TableName in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Проверим, не существует ли уже такая таблица
    if (GetTable(TableName))
    {
        ErrorString == ISAlgorithm::StringF("Table \"%\" already exist", TableName);
        return false;
    }

    //Получаем идентификатор таблицы
    const char *UID = XmlElement->Attribute("UID");
    if (!UID || strlen(UID) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty UID in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Получаем псевдоним
    const char *Alias = XmlElement->Attribute("Alias");
    if (!Alias || strlen(Alias) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty alias in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Получаем локальное имя
    const char *LocalName = XmlElement->Attribute("LocalName");
    if (!LocalName || strlen(LocalName) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty local name in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Получаем локальное имя в множественном числе
    const char *LocalListName = XmlElement->Attribute("LocalListName");
    if (!LocalListName || strlen(LocalListName) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty local list name in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //Заполняем основные поля мета-таблицы
    PMetaTable *MetaTable = new PMetaTable();
    MetaTable->Name = TableName;
    MetaTable->UID = UID;
    MetaTable->Alias = Alias;
    MetaTable->LocalName = LocalName;
    MetaTable->LocalListName = LocalListName;

    //Обработаем системные поля
    if (!InitXSNTableFields(MetaTable, XmlElementTemplateXNS))
    {
        return false;
    }

    //Обработаем пользовательские поля
    if (!InitXSNTableFields(MetaTable, XmlElement->FirstChildElement("Fields")))
    {
        return false;
    }

    //Получаем поле-имя
    const char *TitleName = XmlElement->Attribute("TitleName");
    if (!TitleName || strlen(TitleName) == 0) //Если такого атрибута нет или он пустой - используем ID
    {
        TitleName = "ID";
    }
    else //Атрибут есть - прповеряем наличие указанных полей
    {
        ISVectorString Fields = ISAlgorithm::StringSplit(TitleName, ';');
        for (const std::string &FieldName : Fields)
        {
            if (!MetaTable->Exist(FieldName))
            {
                ErrorString = ISAlgorithm::StringF("Invalid field name \"%s\" in title name. Table name: %s", TitleName, TableName);
                return false;
            }
        }
    }

    MetaTable->TitleName = TitleName;

    Tables.emplace_back(MetaTable);
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
    //Переходим к полям
    tinyxml2::XMLElement *ElementField = XmlElement->FirstChildElement();
    while (ElementField)
    {
        //Получаем идентификатор поля
        const char *UID = ElementField->Attribute("UID");
        if (!UID || strlen(UID) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Empty uid in field. File: %s Line: %d", CurrentXSN, ElementField->GetLineNum());
            return false;
        }

        //Получаем имя поля
        const char *FieldName = ElementField->Attribute("Name");
        if (!FieldName || strlen(FieldName) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Empty field name in field. File: %s Line: %d", CurrentXSN, ElementField->GetLineNum());
            return false;
        }

        //Получаем тип поля
        const char *Type = ElementField->Attribute("Type");
        if (!Type || strlen(Type) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Empty type in field. File: %s Line: %d", CurrentXSN, ElementField->GetLineNum());
            return false;
        }

        PMetaField *MetaField = new PMetaField();
        MetaField->UID = UID;
        MetaField->Name = FieldName;
        MetaField->Type = GetType(ElementField->Attribute("Type"));
        MetaField->Size = ElementField->IntAttribute("Size");
        MetaField->Upper = ElementField->BoolAttribute("Upper");
        MetaField->Lower = ElementField->BoolAttribute("Lower");
        MetaField->DefaultValue = ElementField->FindAttribute("DefaultValue") ? ElementField->Attribute("DefaultValue") : std::string();
        MetaField->DefaultValueWidget = ElementField->FindAttribute("DefaultValueWidget") ? ElementField->Attribute("DefaultValueWidget") : std::string();
        MetaField->LabelName = ElementField->FindAttribute("LabelName") ? ElementField->Attribute("LabelName") : FieldName;
        MetaField->LocalListName = ElementField->FindAttribute("LocalListName") ? ElementField->Attribute("LocalListName") : FieldName;
        MetaField->NotNull = ElementField->BoolAttribute("NotNull");
        MetaField->ReadOnly = ElementField->BoolAttribute("ReadOnly");
        MetaField->HideFromObject = ElementField->BoolAttribute("HideFromObject");
        MetaField->HideFromList = ElementField->BoolAttribute("HideFromList");
        MetaField->NotSearch = ElementField->BoolAttribute("NotSearch");
        MetaField->Hint = ElementField->FindAttribute("Hint") ? ElementField->Attribute("Hint") : std::string();
        MetaField->PlaceholderText = ElementField->FindAttribute("PlaceholderText") ? ElementField->Attribute("PlaceholderText") : std::string();
        MetaField->ControlWidget = ElementField->FindAttribute("ControlWidget") ? ElementField->Attribute("ControlWidget") : std::string();
        MetaField->RegExp = ElementField->FindAttribute("RegExp") ? ElementField->Attribute("RegExp") : std::string();
        MetaField->IsSystem = ElementField->BoolAttribute("IsSystem");
        MetaField->QueryText = ElementField->FindAttribute("QueryText") ? ElementField->Attribute("QueryText") : std::string();
        MetaField->Sequence = ElementField->BoolAttribute("Sequence");
        MetaField->PrimaryKey = ElementField->BoolAttribute("PrimaryKey");
        MetaField->LayoutName = ElementField->FindAttribute("LayoutName") ? ElementField->Attribute("LayoutName") : std::string();
        MetaField->SeparatorName = ElementField->FindAttribute("SeparatorName") ? ElementField->Attribute("SeparatorName") : std::string();
        MetaTable->Fields.emplace_back(MetaField);
        ElementField = ElementField->NextSiblingElement();
    }
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
