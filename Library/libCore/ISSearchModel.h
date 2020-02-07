#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISNamespace.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISSearchModel
{
public:
	ISSearchModel();
	virtual ~ISSearchModel();

	void CreateSearchString(QString &SearchString, QVariantMap &Conditions) const;
	void AddField(const QString &FieldName, const QVariant &Value, ISNamespace::SearchOperatorType Operator, ISNamespace::FieldType FieldType);

protected:
	QString CreateSubString(ISNamespace::SearchOperatorType Operator, const QString &FieldName, const QString &Condition, bool OneField) const;

private:
	QVector<StructModelItem> Vector;
};
//-----------------------------------------------------------------------------
