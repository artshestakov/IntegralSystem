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
    //��������� ���� � �����
    std::string Path = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + ".." + PATH_SEPARATOR + ".." + PATH_SEPARATOR + "Resources" + PATH_SEPARATOR + "Localization" + PATH_SEPARATOR + FileName + ".lang";

    //������
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError Error = XmlDocument.LoadFile(Path.c_str());
    if (Error != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = XmlDocument.ErrorStr();
        return false;
    }

    //��������� � ��������
    tinyxml2::XMLElement *XmlElement = XmlDocument.FirstChildElement()->FirstChildElement();

    //������� ��� �������� ���� �� ������ ���
    while (XmlElement)
    {
        //�������� ���� �������� � ��������� �� ��������
        const char *Key = XmlElement->Attribute("Name");
        auto It = MapUnordered.find(Key);
        if (It == MapUnordered.end()) //������ ����� ��� ��� - ���������
        {
            MapUnordered.emplace(Key, XmlElement->Attribute("Russian"));
        }
        else //��������
        {
            ISLOGGER_W(__CLASS__, "Duplicate key \"%s\"", Key);
        }
        XmlElement = XmlElement->NextSiblingElement(); //��������� � ���������� ��������
    }
    return true;
}
//-----------------------------------------------------------------------------
std::string ISLocalization::GetString(const std::string &Key)
{
    auto It = MapUnordered.find(Key);
    if (It == MapUnordered.end()) //�� ����� ����� ����
    {
        return Key;
    }
    return (*It).second;
}
//-----------------------------------------------------------------------------
