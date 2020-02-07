#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISMetaSetting.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISMetaSettingsGroup
{
public:
	ISMetaSettingsGroup();
	~ISMetaSettingsGroup();

	QString Name;
	ISUuid UID;
	QString LocalName;
	QString IconName;
	bool System;
	QString Hint;
	QVector<ISMetaSetting*> Settings;
};
//-----------------------------------------------------------------------------
