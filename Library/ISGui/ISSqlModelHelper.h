#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISSqlModelHelper
{
public:
	static QVariant ValueForType(const QVariant &Value, ISNamespace::FieldType Type);
	static QVariant ValueFromTextAlignment(ISNamespace::FieldType FieldType, PMetaForeign *MetaForeign); //Конвертировать значение в флаг положения текста
};
//-----------------------------------------------------------------------------
