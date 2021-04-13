#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISAlgorithm.h"
#include "ISDateTime.h"
#include "ISNamespace.h"
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
struct ISTcpClientInfo
{
    std::string IPAddress; //Адрес клиента
    ISDateTime DTConnected; //Дата и время подключения
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
    ISSqlPrepare(const std::string &hash, int parameter_count = 0)
        : Hash(hash), ParameterCount(parameter_count), Prepared(false) { }

    std::string Hash;
    int ParameterCount;
    bool Prepared;
};
//-----------------------------------------------------------------------------
#endif
