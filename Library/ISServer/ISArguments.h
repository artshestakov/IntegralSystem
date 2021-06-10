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
        Unknown,
        Flag,
    };

private:
    struct ISArgumentItem
    {
        ISArgumentItem()
            : Type(ISArguments::ArgumentType::Unknown),
            NotNull(false)
        {

        }

        ISArguments::ArgumentType Type;
        bool NotNull;
        std::string SmallName;
        std::string FullName;
        std::string Description;
    };

public:
    ISArguments(int argc, char **argv);
    ~ISArguments();

    const std::string& GetErrorString() const;
    void AddFlag(const std::string &SmallName, const std::string &FullName, const std::string &Description);

    bool IsExist(const char *Name);
    bool IsEmpty();

private:
    std::string ErrorString;
    int ARGC;
    char **ARGV;
    std::vector<ISArgumentItem> Vector;
};
//-----------------------------------------------------------------------------
#endif
