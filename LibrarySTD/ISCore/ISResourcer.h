#pragma once
#ifndef _ISRESOURCER_H_INCLUDED
#define _ISRESOURCER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISResourcer
{
public:
    ISResourcer();
    ~ISResourcer();

    std::string GetErrorString() const;
    bool LoadFile(const std::string &FilePath);
    void Free();

private:
    bool Parse();

private:
    std::string ErrorString;
    unsigned char *Data;
    long Size;
};
//-----------------------------------------------------------------------------
#endif
