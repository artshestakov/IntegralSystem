#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISAssert.h"
#include "RCC.h"
#include "ISConfigurations.h"
//-----------------------------------------------------------------------------
ISMetaData::ISMetaData()
    : ErrorString(STRING_NO_ERROR),
    Initialized(false),
    CurrentXSN(nullptr),
    CurrentXSR(nullptr),
    CurrentXSF(nullptr),
    TypesCount(0)
{
    //��������� ������ ������ � ����-�������
    VectorXSN.emplace_back(ISResourceItem(RCC::SCHEME_ASTERISK_XSN, RCC::SCHEME_ASTERISK_XSN_SIZE, RCC::SCHEME_ASTERISK_XSN_NAME));
    VectorXSN.emplace_back(ISResourceItem(RCC::SCHEME_SETTINGS_XSN, RCC::SCHEME_SETTINGS_XSN_SIZE, RCC::SCHEME_SETTINGS_XSN_NAME));
    VectorXSN.emplace_back(ISResourceItem(RCC::SCHEME_SETTINGSDATABASE_XSN, RCC::SCHEME_SETTINGSDATABASE_XSN_SIZE, RCC::SCHEME_SETTINGSDATABASE_XSN_NAME));
    VectorXSN.emplace_back(ISResourceItem(RCC::SCHEME_SYSTEM_XSN, RCC::SCHEME_SYSTEM_XSN_SIZE, RCC::SCHEME_SYSTEM_XSN_NAME));
    VectorXSN.emplace_back(ISResourceItem(RCC::SCHEME_TASK_XSN, RCC::SCHEME_TASK_XSN_SIZE, RCC::SCHEME_TASK_XSN_NAME));
    VectorXSN.emplace_back(ISResourceItem(RCC::SCHEME_USER_XSN, RCC::SCHEME_USER_XSN_SIZE, RCC::SCHEME_USER_XSN_NAME));
    VectorXSR.emplace_back(ISResourceItem(RCC::SCHEME_ASTERISK_XSR, RCC::SCHEME_ASTERISK_XSR_SIZE, RCC::SCHEME_ASTERISK_XSR_NAME));
    VectorXSR.emplace_back(ISResourceItem(RCC::SCHEME_SETTINGS_XSR, RCC::SCHEME_SETTINGS_XSR_SIZE, RCC::SCHEME_SETTINGS_XSR_NAME));
    VectorXSR.emplace_back(ISResourceItem(RCC::SCHEME_SETTINGSDATABASE_XSR, RCC::SCHEME_SETTINGSDATABASE_XSR_SIZE, RCC::SCHEME_SETTINGSDATABASE_XSR_NAME));
    VectorXSR.emplace_back(ISResourceItem(RCC::SCHEME_SYSTEM_XSR, RCC::SCHEME_SYSTEM_XSR_SIZE, RCC::SCHEME_SYSTEM_XSR_NAME));
    VectorXSR.emplace_back(ISResourceItem(RCC::SCHEME_TASK_XSR, RCC::SCHEME_TASK_XSR_SIZE, RCC::SCHEME_TASK_XSR_NAME));
    VectorXSR.emplace_back(ISResourceItem(RCC::SCHEME_USER_XSR, RCC::SCHEME_USER_XSR_SIZE, RCC::SCHEME_USER_XSR_NAME));
    VectorXSF.emplace_back(ISResourceItem(RCC::SCHEME_FUNCTIONS_XSF, RCC::SCHEME_FUNCTIONS_XSF_SIZE, RCC::SCHEME_FUNCTIONS_XSF_NAME));

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
const std::string& ISMetaData::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISMetaData::Init(bool XSR, bool XSF)
{
    //���� ����-������ ��� ����������������� - �������
    if (Initialized)
    {
        return true;
    }

    //��������� ������ ������ � ����-������� ������� ������������
    switch (ISConfigurations::Instance().Get().Type)
    {
    case ISNamespace::ConfigurationType::OilSphere:
        VectorXSN.emplace_back(ISResourceItem(RCC::_OILSPHERE_OILSPHERE_XSN, RCC::_OILSPHERE_OILSPHERE_XSN_SIZE, RCC::_OILSPHERE_OILSPHERE_XSN_NAME));
        VectorXSR.emplace_back(ISResourceItem(RCC::_OILSPHERE_OILSPHERE_XSR, RCC::_OILSPHERE_OILSPHERE_XSR_SIZE, RCC::_OILSPHERE_OILSPHERE_XSR_NAME));
        VectorXSF.emplace_back(ISResourceItem(RCC::_OILSPHERE_OILSPHERE_XSF, RCC::_OILSPHERE_OILSPHERE_XSF_SIZE, RCC::_OILSPHERE_OILSPHERE_XSF_NAME));
        break;

    default:
        break;
    }

    if (!XSNInit())
    {
        return false;
    }

    if (XSR && !XSRInit())
    {
        return false;
    }

    if (XSF && !XSFInit())
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
std::string ISMetaData::GetTypeDB(ISNamespace::FieldType FieldType) const
{
    for (size_t i = 0; i < TypesCount; ++i)
    {
        if (VectorTypes[i].TypeField == FieldType)
        {
            return VectorTypes[i].TypeDB;
        }
    }
    IS_ASSERT(false, ISAlgorithm::StringF("Not found type db by type %d", FieldType));
    return std::string();
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
const std::vector<PMetaTable*>& ISMetaData::GetTables() const
{
    return Tables;
}
//-----------------------------------------------------------------------------
const std::vector<PMetaResource*>& ISMetaData::GetResources() const
{
    return Resources;
}
//-----------------------------------------------------------------------------
const std::vector<PMetaFunction*>& ISMetaData::GetFunctions() const
{
    return Functions;
}
//-----------------------------------------------------------------------------
std::vector<PMetaForeign*> ISMetaData::GetForeigns() const
{
    std::vector<PMetaForeign*> Foreigns;
    for (PMetaTable *MetaTable : GetTables()) //����� ������
    {
        for (PMetaField *MetaField : MetaTable->Fields) //����� �����
        {
            if (MetaField->Foreign)
            {
                Foreigns.emplace_back(MetaField->Foreign);
            }
        }
    }
    return Foreigns;
}
//-----------------------------------------------------------------------------
std::vector<PMetaIndex*> ISMetaData::GetIndexes() const
{
    std::vector<PMetaIndex*> Indexes;
    for (PMetaTable *MetaTable : GetTables()) //����� ������
    {
        for (PMetaField *MetaField : MetaTable->Fields) //����� �����
        {
            if (MetaField->Index)
            {
                Indexes.emplace_back(MetaField->Index);
            }
        }

        //��������� ��������� �������
        Indexes.insert(Indexes.end(), MetaTable->IndexesCompound.begin(), MetaTable->IndexesCompound.end());
    }
    return Indexes;
}
//-----------------------------------------------------------------------------
const std::vector<ISResourceItem>& ISMetaData::GetVectorXSN() const
{
    return VectorXSN;
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckExistResource(const std::string &UID) const
{
    for (PMetaResource *MetaResource : Resources)
    {
        if (MetaResource->UID == UID)
        {
            return true;
        }
    }
    return false;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSNInit()
{
    //������ ���������� �������
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError XmlError = XmlDocument.Parse((const char *)RCC::OTHER_CLASSTEMPLATEFIELDS_XML, (size_t)RCC::OTHER_CLASSTEMPLATEFIELDS_XML_SIZE);
    if (XmlError != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = ISAlgorithm::StringF("Not parse template system fields: %s", XmlDocument.ErrorStr());
        return false;
    }

    //�������� ������� ��� �������
    tinyxml2::XMLElement *XmlElementTemplateXNS = XmlDocument.FirstChildElement();
    if (!XmlElementTemplateXNS)
    {
        ErrorString = ISAlgorithm::StringF("Not exist main tag in template system fields");
        return false;
    }

    for (const ISResourceItem &ResourceItem : VectorXSN)
    {
        if (!XSNInit(ResourceItem.Data, ResourceItem.Size, ResourceItem.Name, XmlElementTemplateXNS))
        {
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSNInit(const unsigned char *Content, size_t Size, const std::string &FileName, tinyxml2::XMLElement *XmlElementTemplateXNS)
{
    //������ ���������� �����
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError XmlError = XmlDocument.Parse((const char *)Content, Size);
    if (XmlError != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = ISAlgorithm::StringF("Not parse file \"%s\": %s", FileName.c_str(), XmlDocument.ErrorStr());
        return false;
    }

    //�������� ��� �������� ����
    tinyxml2::XMLElement *XmlElement = XmlDocument.FirstChildElement();
    const char *TagName = XmlElement->Name();
    if (strcmp(TagName, "XSN") != 0)
    {
        ErrorString = ISAlgorithm::StringF("Invalid tag name. \"%s\" in XSN file \"%s\"", TagName, FileName.c_str());
        return false;
    }

    //�������� ������� �������� Name � ������� ����
    const tinyxml2::XMLAttribute *XmlAttribute = XmlElement->FindAttribute("Name");
    if (!XmlAttribute)
    {
        ErrorString = ISAlgorithm::StringF("Not found attribute \"Name\" in a XSN file \"%s\"", FileName.c_str());
        return false;
    }
    CurrentXSN = XmlAttribute->Value();

    //��������� � �������� � ���������� ������
    XmlElement = XmlElement->FirstChildElement();
    while (XmlElement)
    {
        if (!XSNInitTable(XmlElement, XmlElementTemplateXNS))
        {
            return false;
        }
        XmlElement = XmlElement->NextSiblingElement();
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSNInitTable(tinyxml2::XMLElement *XmlElement, tinyxml2::XMLElement *XmlElementTemplateXNS)
{
    //��������� ������� ��������� �������
    if (!XmlElement->FirstAttribute())
    {
        ErrorString = ISAlgorithm::StringF("Empty attributes table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //�������� ��� �������
    const char *TableName = XmlElement->Attribute("Name");
    if (!TableName || strlen(TableName) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty name in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //��������, �� ���������� �� ��� ����� �������
    if (GetTable(TableName))
    {
        ErrorString == ISAlgorithm::StringF("Table \"%\" already exist", TableName);
        return false;
    }

    //�������� ������������� �������
    const char *UID = XmlElement->Attribute("UID");
    if (!UID || strlen(UID) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty UID in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //�������� ���������
    const char *Alias = XmlElement->Attribute("Alias");
    if (!Alias || strlen(Alias) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty alias in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //�������� ��������� ���
    const char *LocalName = XmlElement->Attribute("LocalName");
    if (!LocalName || strlen(LocalName) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty local name in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //�������� ��������� ��� � ������������� �����
    const char *LocalListName = XmlElement->Attribute("LocalListName");
    if (!LocalListName || strlen(LocalListName) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty local list name in table. File: %s Line: %d", CurrentXSN, XmlElement->GetLineNum());
        return false;
    }

    //��������� �������� ���� ����-�������
    PMetaTable *MetaTable = new PMetaTable();
    MetaTable->Name = TableName;
    MetaTable->UID = UID;
    MetaTable->Alias = Alias;
    MetaTable->LocalName = LocalName;
    MetaTable->LocalListName = LocalListName;

    //���������� ��������� ����
    if (!XSNInitTableFields(MetaTable, XmlElementTemplateXNS))
    {
        return false;
    }

    //���������� ���������������� ����
    if (!XSNInitTableFields(MetaTable, XmlElement->FirstChildElement("Fields")))
    {
        return false;
    }

    //���������� �������
    if (!XSNInitIndexes(MetaTable, XmlElement->FirstChildElement("Indexes")))
    {
        return false;
    }

    if (!XSNInitForeigns(MetaTable, XmlElement->FirstChildElement("Foreigns")))
    {
        return false;
    }

    if (!XSNInitEscorts(MetaTable, XmlElement->FirstChildElement("Escorts")))
    {
        return false;
    }

    //�������� ����-���
    const char *TitleName = XmlElement->Attribute("TitleName");
    if (!TitleName || strlen(TitleName) == 0) //���� ������ �������� ��� ��� �� ������ - ���������� ID
    {
        TitleName = "ID";
    }
    else //������� ���� - ���������� ������� ��������� �����
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

    //�������� ���� ����������� ����
    const char *IsSystem = XmlElement->Attribute("IsSystem");
    if (IsSystem && strlen(IsSystem) > 0)
    {
        std::string Temp(IsSystem);
        ISAlgorithm::StringToLower(Temp);
        MetaTable->IsSystem = Temp == "true";
    }

    //�������� ���� "������ ��������"
    const char *ShowOnly = XmlElement->Attribute("ShowOnly");
    if (ShowOnly && strlen(ShowOnly) > 0)
    {
        std::string Temp(ShowOnly);
        ISAlgorithm::StringToLower(Temp);
        MetaTable->ShowOnly = Temp == "true";
    }

    Tables.emplace_back(MetaTable);
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSNInitTableFields(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement)
{
    //��������� � �����
    tinyxml2::XMLElement *ElementField = XmlElement->FirstChildElement();
    while (ElementField)
    {
        //��������� ������� ��������� ����
        if (!ElementField->FirstAttribute())
        {
            ErrorString = ISAlgorithm::StringF("Empty attributes field. File: %s Line: %d", CurrentXSN, ElementField->GetLineNum());
            return false;
        }

        //�������� ������������� ����
        const char *UID = ElementField->Attribute("UID");
        if (!UID || strlen(UID) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Empty uid in field. File: %s Line: %d", CurrentXSN, ElementField->GetLineNum());
            return false;
        }

        //�������� ��� ����
        const char *FieldName = ElementField->Attribute("Name");
        if (!FieldName || strlen(FieldName) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Empty field name in field. File: %s Line: %d", CurrentXSN, ElementField->GetLineNum());
            return false;
        }

        //�������� ��� ����
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
bool ISMetaData::XSNInitIndexes(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement)
{
    if (!XmlElement) //� ���� ������� ��� ��������
    {
        return true;
    }

    //��������� � ��������
    tinyxml2::XMLElement *ElementIndex = XmlElement->FirstChildElement();
    while (ElementIndex)
    {
        //��������� ������� ��������� �������
        if (!ElementIndex->FirstAttribute())
        {
            ErrorString = ISAlgorithm::StringF("Empty attributes index. File: %s Line: %d", CurrentXSN, ElementIndex->GetLineNum());
            return false;
        }

        //�������� ���� �������
        const char *Fields = ElementIndex->Attribute("Field");
        if (!Fields || strlen(Fields) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Empty index name. File: %s Line: %d", CurrentXSN, ElementIndex->GetLineNum());
            return false;
        }

        //�������� ������������ �������
        bool Unique = ElementIndex->BoolAttribute("Unique");

        size_t FieldsCount = 0;
        ISVectorString VectorString = ISAlgorithm::StringSplit(Fields, ';', FieldsCount);
        if (FieldsCount > 1) //������ ���������
        {
            PMetaIndex *Index = new PMetaIndex(Unique, MetaTable->Alias, MetaTable->Name, std::string());
            for (const std::string &FieldName : VectorString)
            {
                if (!MetaTable->GetField(FieldName))
                {
                    ErrorString = ISAlgorithm::StringF("Field \"%s\" not found", FieldName.c_str());
                    return false;
                }
                Index->Fields.emplace_back(FieldName);
            }
            MetaTable->IndexesCompound.emplace_back(Index);
        }
        else //������ �����������
        {
            //�������� ��� ���� � ����-����
            std::string FieldName = VectorString.front();
            PMetaField *MetaField = MetaTable->GetField(VectorString.front());
            if (!MetaField) //��� ������ ����
            {
                ErrorString = ISAlgorithm::StringF("Field \"%s\" not found", FieldName.c_str());
                return false;
            }

            //��������, ���� �� �� ���� ���� ��� ������
            if (MetaField->Index)
            {
                ErrorString = ISAlgorithm::StringF("Index on the field \"%s\" already exist", FieldName.c_str());
                return false;
            }
            MetaField->Index = new PMetaIndex(Unique, MetaTable->Alias, MetaTable->Name, FieldName);
        }
        ElementIndex = ElementIndex->NextSiblingElement();
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSNInitForeigns(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement)
{
    if (!XmlElement) //� ���� ������� ��� ������� ������
    {
        return true;
    }

    //��������� � ������� ������
    tinyxml2::XMLElement *ElementForeign = XmlElement->FirstChildElement();
    while (ElementForeign)
    {
        //��������� ������� ��������� �������� �����
        if (!ElementForeign->FirstAttribute())
        {
            ErrorString = ISAlgorithm::StringF("Empty attributes foreign key. File: %s Line: %d", CurrentXSN, ElementForeign->GetLineNum());
            return false;
        }

        //�������� ��� ����, �� ������� ��������������� ������� ����
        const char *FieldName = ElementForeign->Attribute("Field");
        if (!FieldName || strlen(FieldName) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Empty foreign field name. File: %s Line: %d", CurrentXSN, ElementForeign->GetLineNum());
            return false;
        }

        //�������� ��� �������, �� ������� ��������� ������� ����
        const char *ForeignClass = ElementForeign->Attribute("ForeignClass");
        if (!ForeignClass || strlen(ForeignClass) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Empty foreign class. File: %s Line: %d", CurrentXSN, ElementForeign->GetLineNum());
            return false;
        }

        //�������� ��� ����, �� ������� ��������� ������� ����
        const char *ForeignField = ElementForeign->Attribute("ForeignField");
        if (!ForeignField || strlen(ForeignField) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Empty foreign field. File: %s Line: %d", CurrentXSN, ElementForeign->GetLineNum());
            return false;
        }

        //�������� ������� ���� - �� ������� �������� ������� ����
        PMetaField *MetaField = MetaTable->GetField(FieldName);
        if (!MetaField)
        {
            ErrorString = ISAlgorithm::StringF("Field \"%s\" not found", FieldName);
            break;
        }

        //�������� ������� �������� ����� �� ���� ����
        if (MetaField->Foreign)
        {
            ErrorString = ISAlgorithm::StringF("Foreign key on the field \"%s\" already exist", FieldName);
            break;
        }

        //�������� ����, ������� ����� ����������� � ����� �������� �������
        const char *ForeignViewFieldName = ElementForeign->Attribute("ForeignViewNameField");

        MetaField->Foreign = new PMetaForeign(FieldName, ForeignClass, ForeignField,
            ForeignViewFieldName ? ForeignViewFieldName : std::string(), MetaTable->Name);
        ElementForeign = ElementForeign->NextSiblingElement();
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSNInitEscorts(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement)
{
    if (!XmlElement) //� ���� ������� ��� ��������
    {
        return true;
    }

    //��������� � ��������
    tinyxml2::XMLElement *ElementEscort = XmlElement->FirstChildElement();
    while (ElementEscort)
    {
        //��������� ������� ��������� �������� �����
        if (!ElementEscort->FirstAttribute())
        {
            ErrorString = ISAlgorithm::StringF("Empty attributes escort. File: %s Line: %d", CurrentXSN, ElementEscort->GetLineNum());
            return false;
        }

        const char *LocalName = ElementEscort->Attribute("LocalName");
        const char *TableName = ElementEscort->Attribute("TableName");
        const char *FilterField = ElementEscort->Attribute("FilterField");
        const char *ClassName = ElementEscort->Attribute("ClassName");

        PMetaEscort *MetaEscort = new PMetaEscort();
        MetaEscort->LocalName = LocalName ? LocalName : std::string();
        MetaEscort->TableName = TableName ? TableName : std::string();
        MetaEscort->FilterField = FilterField ? FilterField : std::string();
        MetaEscort->ClassName = ClassName ? ClassName : std::string();
        MetaTable->Escorts.emplace_back(MetaEscort);
        ElementEscort = ElementEscort->NextSiblingElement();
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSRInit()
{
    for (const ISResourceItem &ResourceItem : VectorXSR)
    {
        if (!XSRInit(ResourceItem.Data, ResourceItem.Size, ResourceItem.Name))
        {
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSRInit(const unsigned char *Content, size_t Size, const std::string &FileName)
{
    //������ ���������� �����
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError XmlError = XmlDocument.Parse((const char *)Content, Size);
    if (XmlError != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = ISAlgorithm::StringF("Not parse file \"%s\": %s", FileName.c_str(), XmlDocument.ErrorStr());
        return false;
    }

    //�������� ��� �������� ����
    tinyxml2::XMLElement *XmlElement = XmlDocument.FirstChildElement();
    const char *TagName = XmlElement->Name();
    if (strcmp(TagName, "XSR") != 0)
    {
        ErrorString = ISAlgorithm::StringF("Invalid tag name. \"%s\" in XSR file \"%s\"", TagName, FileName.c_str());
        return false;
    }

    //�������� ������� �������� Name � ������� ����
    const tinyxml2::XMLAttribute *XmlAttribute = XmlElement->FindAttribute("Name");
    if (!XmlAttribute)
    {
        ErrorString = ISAlgorithm::StringF("Not found attribute \"Name\" in a XSR file \"%s\"", FileName.c_str());
        return false;
    }
    CurrentXSR = XmlAttribute->Value();

    //��������� � �������� � ���������� ������
    XmlElement = XmlElement->FirstChildElement();
    while (XmlElement)
    {
        if (!XSRInit(XmlElement))
        {
            return false;
        }
        XmlElement = XmlElement->NextSiblingElement();
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSRInit(tinyxml2::XMLElement *XmlElement)
{
    //�������� ��� �������
    const char *TableName = XmlElement->Name();

    //�������� � �������
    if (!GetTable(TableName))
    {
        ErrorString = ISAlgorithm::StringF("Table name \"%s\" not exist. File: %s Line: %d", TableName, CurrentXSR, XmlElement->GetLineNum());
        return false;
    }

    PMetaResource *MetaResource = new PMetaResource();
    MetaResource->TableName = TableName;

    //�������� ������� ����������� ��������������
    const tinyxml2::XMLAttribute *XmlAttributeUID = XmlElement->FindAttribute("UID");
    if (!XmlAttributeUID)
    {
        ErrorString = ISAlgorithm::StringF("Not found resource UID. File: %s Line: %d", CurrentXSR, XmlElement->GetLineNum());
        return false;
    }

    //�������� ������������� �������, ��������� � ������ ������� � �������� �������� ������
    std::string ResourceUID = XmlElement->Attribute("UID");
    ISAlgorithm::StringToLower(ResourceUID);
    ResourceUID = ResourceUID.substr(1, ResourceUID.size() - 2);
    MetaResource->UID = ResourceUID;

    //������� ������� � ���������������, ����� ����� �������� �� ���� �����
    XmlElement->DeleteAttribute("UID");

    const tinyxml2::XMLAttribute *XmlAttribute = XmlElement->FirstAttribute();
    do 
    {
        MetaResource->AddField(XmlAttribute->Name(), XmlAttribute->Value());
    } while ((XmlAttribute = XmlAttribute->Next()) != nullptr);

    Resources.emplace_back(MetaResource);
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSFInit()
{
    for (const ISResourceItem &ResourceItem : VectorXSF)
    {
        if (!XSFInit(ResourceItem.Data, ResourceItem.Size, ResourceItem.Name))
        {
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSFInit(const unsigned char *Content, size_t Size, const std::string &FileName)
{
    //������ ���������� �����
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError XmlError = XmlDocument.Parse((const char *)Content, Size);
    if (XmlError != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = ISAlgorithm::StringF("Not parse file \"%s\": %s", FileName.c_str(), XmlDocument.ErrorStr());
        return false;
    }

    //�������� ��� �������� ����
    tinyxml2::XMLElement *XmlElement = XmlDocument.FirstChildElement();
    const char *TagName = XmlElement->Name();
    if (strcmp(TagName, "XSF") != 0)
    {
        ErrorString = ISAlgorithm::StringF("Invalid tag name. \"%s\" in XSF file \"%s\"", TagName, FileName.c_str());
        return false;
    }

    //�������� ������� �������� Name � ������� ����
    const tinyxml2::XMLAttribute *XmlAttribute = XmlElement->FindAttribute("Name");
    if (!XmlAttribute)
    {
        ErrorString = ISAlgorithm::StringF("Not found attribute \"Name\" in a XSF file \"%s\"", FileName.c_str());
        return false;
    }
    CurrentXSF = XmlAttribute->Value();

    //��������� � �������� � ���������� ������
    XmlElement = XmlElement->FirstChildElement();
    while (XmlElement)
    {
        if (!XSFInit(XmlElement))
        {
            return false;
        }
        XmlElement = XmlElement->NextSiblingElement();
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISMetaData::XSFInit(tinyxml2::XMLElement *XmlElement)
{
    //��������� ������� ���������
    if (!XmlElement->FirstAttribute())
    {
        ErrorString = ISAlgorithm::StringF("Empty attributes function. File: %s Line: %d", CurrentXSF, XmlElement->GetLineNum());
        return false;
    }

    //�������� ��� �������
    const char *Name = XmlElement->Attribute("Name");
    if (!Name || strlen(Name) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty function name in function. File: %s Line: %d", CurrentXSF, XmlElement->GetLineNum());
        return false;
    }

    //�������� ����� �������
    const char *SqlText = XmlElement->GetText();
    if (!SqlText || strlen(SqlText) == 0)
    {
        ErrorString = ISAlgorithm::StringF("Empty sql text in function. File: %s Line: %d", CurrentXSF, XmlElement->GetLineNum());
        return false;
    }

    //�������� �����������
    const char *Comment = XmlElement->Attribute("Comment");    
    Functions.emplace_back(new PMetaFunction{ Name, Comment ? Comment : std::string() , SqlText });
    return true;
}
//-----------------------------------------------------------------------------
