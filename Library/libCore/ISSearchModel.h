#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISSearchModel
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
