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
        std::string Name;
        std::string Description;
    };

public:
    ISArguments(int argc, char **argv);
    ~ISArguments();

    const std::string& GetErrorString() const;
    void AddFlag(const std::string &Name, const std::string &Description);

    bool Exist(const char *Name);

private:
    std::string ErrorString;
    int ARGC;
    char **ARGV;
    std::vector<ISArgumentItem> Vector;
};
//-----------------------------------------------------------------------------
#endif
