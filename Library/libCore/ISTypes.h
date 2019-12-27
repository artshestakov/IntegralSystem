#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISMetaDataType
{
	QString TypeName;
	ISNamespace::FieldType TypeField;
	QString TypeDB;
	QString ControlWidget;
	QString SearchOperatorWidget;
	bool Search;
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT StructModelItem
{
	QString FieldName;
	QVector<QVariant> Values;
	ISNamespace::SearchOperatorType Operator;
	ISNamespace::FieldType FieldType;
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISRangeStruct
{
public:
	ISRangeStruct();
	ISRangeStruct(const QVariant &begin_value, const QVariant &end_value);

	void Clear();
	bool IsValid() const;

	QVariant BeginValue;
	QVariant EndValue;
};
//-----------------------------------------------------------------------------
typedef QMap<QString, QString> QStringMap;
typedef QVector<int> QVectorInt;
typedef QVector<QString> QVectorString;
//-----------------------------------------------------------------------------
