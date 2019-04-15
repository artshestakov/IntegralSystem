#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISNamespace.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISSearchModel : public QObject
{
	Q_OBJECT

public:
	ISSearchModel(QObject *parent = 0);
	virtual ~ISSearchModel();

	void CreateSearchString(QString &SearchString, QVariantMap &Conditions) const;
	void AddField(const QString &FieldName, const QVariant &Value, ISNamespace::SearchOperatorType Operator, ISNamespace::FieldType FieldType);

protected:
	QString CreateSubString(ISNamespace::SearchOperatorType Operator, const QString &FieldName, const QString &Condition, bool OneField) const;

private:
	QVector<StructModelItem> Vector;
};
//-----------------------------------------------------------------------------
