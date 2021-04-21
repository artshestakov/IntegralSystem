#include "ISLocalization.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "tinyxml2.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISLocalization::ISLocalization()
    : ErrorString(STRING_NO_ERROR)
{

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
std::string ISLocalization::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLocalization::Init(const std::string &FileName)
{
    //Формируем путь к файлу
    std::string Path = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + ".." + PATH_SEPARATOR + ".." + PATH_SEPARATOR + "Resources" + PATH_SEPARATOR + "Localization" + PATH_SEPARATOR + FileName + ".lang";

    //Парсим
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError Error = XmlDocument.LoadFile(Path.c_str());
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
        auto It = MapUnordered.find(Key);
        if (It == MapUnordered.end()) //Такого ключа ещё нет - добавляем
        {
            MapUnordered.emplace(Key, XmlElement->Attribute("Russian"));
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
std::string ISLocalization::GetString(const std::string &Key)
{
    auto It = MapUnordered.find(Key);
    if (It == MapUnordered.end()) //Не нашли такой ключ
    {
        return Key;
    }
    return (*It).second;
}
//-----------------------------------------------------------------------------
