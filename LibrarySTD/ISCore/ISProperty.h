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

    const uint64_t& GetUptime() const;
    void SetUptime(uint64_t uptime);

private:
    ISProperty();
    ~ISProperty();
    ISProperty(const ISProperty&) = delete;
    ISProperty(ISProperty&&) = delete;
    ISProperty& operator=(const ISProperty&) = delete;
    ISProperty& operator=(ISProperty&&) = delete;

private:
    uint64_t Uptime;
};
//-----------------------------------------------------------------------------
#endif
