#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISDateTime.h"
#include "ISNamespace.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
struct ISConfigParameter
{
    std::string TemplateName; //Имя шаблона
    std::string SectionName; //Имя секции
    std::string Name; //Имя параметра
    std::string Type; //Тип параметра
    bool NotNull; //Параметр обязателен для заполнения
    std::string DefaultValue; //Значение по умолчанию
    int Minimum; //Минимальное значение (для числовых параметров)
    int Maximum; //Максимальное значение (для числовых параметров)
};
//-----------------------------------------------------------------------------
struct ISConnectOptionDB
{
    ISConnectOptionDB() : Port(0) { }

    ISConnectOptionDB(const std::string& host, unsigned short port, const std::string& name, const std::string& login, const std::string& password)
        : Host(host), Port(port), Name(name), Login(login), Password(password) { }

    std::string Host; //Адрес сервера
    unsigned short Port; //Порт
    std::string Name; //Имя базы данных
    std::string Login; //Логин пользователя
    std::string Password; //Пароль
};
//-----------------------------------------------------------------------------
struct ISResourceFile
{
    ISResourceFile()
        : Size(0), Data(nullptr) { }

    ISResourceFile(const std::string &name, unsigned long size, char *data)
        : Name(name), Size(size), Data(data) { }

    std::string Name;
    unsigned long Size;
    char *Data;
};
//-----------------------------------------------------------------------------
struct ISMetaType
{
    ISMetaType(const std::string& type_name, ISNamespace::FieldType type, const std::string& type_db, const std::string& control_widget, const std::string& search_condition_widget, bool search_allowed)
        : TypeName(type_name), TypeField(type), TypeDB(type_db), ControlWidget(control_widget), SearchConditionWidget(search_condition_widget), SearchAllowed(search_allowed)
    { }

    std::string TypeName; //Наименование типа
    ISNamespace::FieldType TypeField; //Тип даннных в системе
    std::string TypeDB; //Тип данных в базе
    std::string ControlWidget; //Имя поля редактирования
    std::string SearchConditionWidget; //Имя поискового виджета
    bool SearchAllowed; //Возможность поиска по полю
};
//-----------------------------------------------------------------------------
struct ISSqlPrepare
{
    std::string SqlText;
    std::string Hash;
#ifdef WIN32
    unsigned ThreadID;
#else
    pthread_t ThreadID;
#endif
    int ParameterCount;
};
//-----------------------------------------------------------------------------
struct ISConfigurationInfo
{
    std::string Name; //Наименование
    std::string UID; //Идентификатор
    std::string LocalName; //Локальное наименование
    std::string DesktopForm; //Имя класса рабочего стола
    ISDate DateExpired; //Дата, после которой программа запускаться не должна
    std::string LogoName;
};
//-----------------------------------------------------------------------------
struct ISFileInfo
{
    ISFileInfo()
        : Size(0) { }

    std::string Path; //Путь к файлу
    std::string Name; //Имя файла
    ISDateTime DateTimeEdit; //Дата изменения файла
    unsigned long Size; //Размер файла
};
//-----------------------------------------------------------------------------
struct ISLocal
{
    ISLocal()
        : Number(0) { }

    char Value[256];
    int Number;
};
//-----------------------------------------------------------------------------
struct ISLockedIP
{
    ISLockedIP(const std::string &ip_address)
        : IPAddress(ip_address),
        AttemptLeft(CARAT_AUTH_FAIL_LIMIT),
        DateTimeUnlock(ISDateTime::GetCurrentUnixtime() + (CARAT_AUTH_MINUTE_LOCK * 60))
    { }

    bool IsLock() const
    {
        return AttemptLeft == 0;
    }

    std::string IPAddress; //IP-адрес
    size_t AttemptLeft; //Осталось попыток
    uint64_t DateTimeUnlock; //Дата и время разблокировки
};
//-----------------------------------------------------------------------------
struct CGSectionItem
{
    std::string Function;
    std::string Description;
};
//-----------------------------------------------------------------------------
struct CGSection
{
    std::string Name;
    std::string ClassName;
    std::vector<CGSectionItem> Items;
};
//-----------------------------------------------------------------------------
#endif
