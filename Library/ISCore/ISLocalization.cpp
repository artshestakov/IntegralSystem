#include "ISLocalization.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "tinyxml2.h"
#include "ISLogger.h"
#include "ISResourcer.h"
//-----------------------------------------------------------------------------
ISLocalization::ISLocalization()
    : ErrorString(STRING_NO_ERROR),
    Map(nullptr)
{
    Map = hashmap_new();
    if (!Map)
    {
        //error
    }
}
//-----------------------------------------------------------------------------
ISLocalization::~ISLocalization()
{

}
//-----------------------------------------------------------------------------
ISLocalization& ISLocalization::Instance()
{
    static ISLocalization Localization;
    return Localization;
}
//-----------------------------------------------------------------------------
const std::string& ISLocalization::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLocalization::InitFile(const std::string &FileName)
{
    //Получаем содержимое файла
    unsigned long FileSize = 0;
    const char *Data = ISResourcer::Instance().GetFile(ISAlgorithm::StringF("Localization/%s.lang", FileName.c_str()), FileSize);
    return Init(Data, (size_t)FileSize);
}
//-----------------------------------------------------------------------------
bool ISLocalization::InitContent(const char *Content)
{
    return Init(Content, strlen(Content));
}
//-----------------------------------------------------------------------------
bool ISLocalization::Init(const char *Data, size_t Size)
{
    //Парсим 
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError Error = XmlDocument.Parse(Data, Size);
    if (Error != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = XmlDocument.ErrorStr();
        return false;
    }

    //Переходим к контенту
    tinyxml2::XMLElement *XmlElement = XmlDocument.FirstChildElement()->FirstChildElement();

    //Обходим все элементы пока не обойдём все
    while (XmlElement)
    {
        //Получаем ключ перевода и проверяем на дубликат
        const char *Key = XmlElement->Attribute("Name");
        any_t Any;
        if (hashmap_get(Map, (char *)Key, &Any) == MAP_MISSING) //Такого ключа ещё нет - добавляем
        {
            const char *Value = XmlElement->Attribute("Russian");

            //Выделяем память под ключ и значение
            char *KKey = (char *)malloc(sizeof(char) * strlen(Key) + 1),
                *VValue = (char *)malloc(sizeof(Value) * strlen(Value) + 1);

            strcpy(KKey, Key);
            strcpy(VValue, Value);

            if (hashmap_put(Map, KKey, VValue) != MAP_OK)
            {
                ISLOGGER_W(__CLASS__, "Error insert key \"%s\"", Key);
                break;
            }
        }
        else //Дубликат
        {
            ISLOGGER_W(__CLASS__, "Duplicate key \"%s\"", Key);
        }
        XmlElement = XmlElement->NextSiblingElement(); //Переходим к следующему элементу
    }
    return true;
}
//-----------------------------------------------------------------------------
const char* ISLocalization::GetString(const char *Key)
{
    ISLocal *Local = nullptr;
    if (hashmap_get(Map, (char *)Key, (void **)&Local) == MAP_OK)
    {
        return Local->Value;
    }
    return Key;
}
//-----------------------------------------------------------------------------
