#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISNamespace.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISSqlModelHelper : public QObject
{
	Q_OBJECT

public:
	ISSqlModelHelper();
	virtual ~ISSqlModelHelper();

	static QVariant ValueForType(const QVariant &Value, ISNamespace::FieldType Type);
	static QVariant ValueFromTextAlignment(ISNamespace::FieldType FieldType, PMetaClassForeign *MetaForeign); //Конвертировать значение в флаг положения текста
	static QString ValueForToolTip(bool ShowToolTip, const QVariant &Value, ISNamespace::FieldType FieldType); //Конвертировать значение в текст для подсказки
};
//-----------------------------------------------------------------------------
