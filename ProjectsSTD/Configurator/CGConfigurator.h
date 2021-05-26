#pragma once
#ifndef _CGCONFIGURATOR_H_INCLUDED
#define _CGCONFIGURATOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class CGConfigurator : public CGConfiguratorBase
{
public:
    CGConfigurator(const std::vector<CGSection> &arguments);
    virtual ~CGConfigurator();

public:
    bool help(); //Показать помощь

private:
    std::vector<CGSection> Arguments;
};
//-----------------------------------------------------------------------------
#endif
