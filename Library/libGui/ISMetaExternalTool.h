#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
struct ISMetaExternalTool
{
public:
	ISMetaExternalTool();
	~ISMetaExternalTool();

	QString LocalName;
	QString Command;
	ISUuid UID;
	QIcon Icon;
};
//-----------------------------------------------------------------------------
