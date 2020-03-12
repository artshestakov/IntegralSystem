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
	static QString ValueForToolTip(bool ShowToolTip, const QVariant &Value, ISNamespace::FieldType FieldType); //Конвертировать значение в текст для подсказки
};
//-----------------------------------------------------------------------------
