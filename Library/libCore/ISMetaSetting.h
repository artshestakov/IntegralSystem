#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISNamespace.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISMetaSetting
{
public:
	ISMetaSetting();
	~ISMetaSetting();

	ISUuid UID;
	QString Name;
	ISNamespace::FieldType SettingType;
	QString WidgetEditName;
	QString LocalName;
	QString Hint;
	QVariant DefaultValue;
	QVariant Value;
};
//-----------------------------------------------------------------------------
