#pragma once
#ifndef _ISCARATMONITOR_H_INCLUDED
#define _ISCARATMONITOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCaratMonitor
{
public:
	static ISCaratMonitor& Instance();

private:
	ISCaratMonitor();
	~ISCaratMonitor();
	ISCaratMonitor(ISCaratMonitor const &) {};
	ISCaratMonitor& operator=(ISCaratMonitor const&) { return *this; };
};
//-----------------------------------------------------------------------------
#endif
