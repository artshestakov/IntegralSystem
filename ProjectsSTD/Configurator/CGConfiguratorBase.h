#pragma once
#ifndef _CGCONFIGURATORBASE_H_INCLUDED
#define _CGCONFIGURATORBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGConfiguratorBase
{
public:
    CGConfiguratorBase();
    virtual ~CGConfiguratorBase();

    const std::string& GetErrorString() const;

protected:
    void Progress(const std::string &ObjectTypeName, size_t Step, size_t TotalStep, const std::string &Description = std::string());

protected:
    std::string ErrorString;
};
//-----------------------------------------------------------------------------
#endif
