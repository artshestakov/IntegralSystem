#pragma once
#ifndef _ISPROPERTY_H_INCLUDED
#define _ISPROPERTY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISProperty
{
public:
    static ISProperty& Instance();

    const ISUInt64& GetUptime() const;
    void SetUptime(ISUInt64 uptime);

private:
    ISProperty();
    ~ISProperty();
    ISProperty(const ISProperty&) = delete;
    ISProperty(ISProperty&&) = delete;
    ISProperty& operator=(const ISProperty&) = delete;
    ISProperty& operator=(ISProperty&&) = delete;

private:
    ISUInt64 Uptime;
};
//-----------------------------------------------------------------------------
#endif
