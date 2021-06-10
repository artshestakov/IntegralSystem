#pragma once
#ifndef _ISARGUMENTS_H_INCLUDED
#define _ISARGUMENTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISArguments
{
public:
    enum class ArgumentType //Тип аргументов командной строки
    {
        Unknown, //Неизвестно
        Flag, //Флаг
        Parameter, //Параметр
    };

private:
    struct ISArgumentItem
    {
        ISArgumentItem()
            : Type(ArgumentType::Unknown),
            Enabled(false)
        {

        }

        ISArguments::ArgumentType Type; //Тип аргумента
        bool Enabled;
        std::string SmallName; //Упрощенное название
        std::string FullName; //Полное название
        std::string Description; //Описание
        std::string Value; //Значение (заполняется только когда Type=ArgumentType::Parameter)
    };

public:
    ISArguments();
    ~ISArguments();

    const std::string& GetErrorString() const;
    std::string GetHelp() const;
    void AddFlag(const std::string &SmallName, const std::string &FullName, const std::string &Description);
    void AddParameter(const std::string &SmallName, const std::string &FullName, const std::string &Description);
    bool Parse(int argc, char **argv);

    bool IsExist(const std::string &Name);
    bool IsEmpty();
    std::string GetValue(const std::string &Name);

private:
    void Add(ArgumentType Type, const std::string &SmallName, const std::string &FullName, const std::string &Description);
    bool ExistInVector(const std::string &Name, ArgumentType &Type);
    void SetValue(const std::string &Name, const std::string &Value);
    void SetEnabled(const std::string &Name);

private:
    std::string ErrorString;
    std::vector<ISArgumentItem> Vector;
};
//-----------------------------------------------------------------------------
#endif
