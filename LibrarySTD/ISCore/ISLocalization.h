#pragma once
#ifndef _ISLOCALIZATION_H_INCLUDED
#define _ISLOCALIZATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISLocalization
{
public:
    static ISLocalization& Instance();

    std::string GetErrorString() const;
    bool Init(const std::string &FileName);
    std::string GetString(const std::string &Key);

private:
    ISLocalization();
    ~ISLocalization();
    ISLocalization(const ISLocalization&) = delete;
    ISLocalization(ISLocalization&&) = delete;
    ISLocalization& operator=(const ISLocalization&) = delete;
    ISLocalization& operator=(ISLocalization&&) = delete;

private:
    std::string ErrorString;
    ISMapString Map;
};
//-----------------------------------------------------------------------------
#define LANG(KEY) ISLocalization::Instance().GetString(KEY)
//-----------------------------------------------------------------------------
#endif
