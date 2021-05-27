#pragma once
#ifndef _CGCONFIGURATORBASE_H_INCLUDED
#define _CGCONFIGURATORBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGConfiguratorBase
{
protected:
    typedef bool (CGConfiguratorBase::*Function)();

public:
    CGConfiguratorBase();
    virtual ~CGConfiguratorBase();

    const std::string& GetErrorString() const;
    bool Invoke(const std::string &FunctionName);
    bool ExistFunction(const std::string &FunctionName);

protected:
    void Progress(const std::string &ObjectTypeName, size_t Step, size_t TotalStep, const std::string &Description = std::string());
    void RegisterFunction(const std::string &FunctionName, Function f);

protected:
    std::string ErrorString;

    std::unordered_map<std::string, Function> MapFunction;
};
//-----------------------------------------------------------------------------
#endif
