#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISObjectModelItem.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISObjectModel : public QObject
{
	Q_OBJECT

public:
	ISObjectModel(QObject *parent = 0);
	virtual ~ISObjectModel();

	void Append(const QString &FieldName);
	
	QVariant GetOldValue(const QString &FieldName) const;
	void SetOldValue(const QString &FieldName, const QVariant &Value);

	QVariant GetNewValue(const QString &FieldName) const;
	void SetNewValue(const QString &FieldName, const QVariant &Value);

private:
	QMap<QString, ISObjectModelItem*> Items;
};
//-----------------------------------------------------------------------------
