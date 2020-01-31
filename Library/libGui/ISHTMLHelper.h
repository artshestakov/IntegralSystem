#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
class ISHTMLHelper
{
public:
	static QString GenerateTableFromQuerySelect(const QString &QueryText);
	static QString GenerateTableFromQuerySelect(ISQuery Query);
};
//-----------------------------------------------------------------------------
