#pragma once
#ifndef _CGCONFIGURATORSERVICE_H_INCLUDED
#define _CGCONFIGURATORSERVICE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class CGConfiguratorService : public CGConfiguratorBase
{
public:
    CGConfiguratorService();
    virtual ~CGConfiguratorService();

    //bool reindex();
    //bool vacuum();
    //bool vacuumanalyze();
    //bool vacuumfull();
    //bool cleartable();
};
//-----------------------------------------------------------------------------
#endif
