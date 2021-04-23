#pragma once
#ifndef _ISLOCALIZATION_H_INCLUDED
#define _ISLOCALIZATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
#include "hashmap.h"
//-----------------------------------------------------------------------------
class ISLocalization
{
public:
    static ISLocalization& Instance();

    std::string GetErrorString() const;
    bool Init(const std::string &FileName);
    const char* GetString(const char *Key);

private:
    ISLocalization();
    ~ISLocalization();
    ISLocalization(const ISLocalization&) = delete;
    ISLocalization(ISLocalization&&) = delete;
    ISLocalization& operator=(const ISLocalization&) = delete;
    ISLocalization& operator=(ISLocalization&&) = delete;

private:
    std::string ErrorString;
    map_t Map;
};
//-----------------------------------------------------------------------------
#define LANG(KEY) ISLocalization::Instance().GetString(KEY)
//-----------------------------------------------------------------------------
#endif
