#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISObjectModel
{
public:
	ISObjectModel();
	~ISObjectModel();

	void Append(const QString &FieldName);
	
	QVariant GetOldValue(const QString &FieldName) const;
	void SetOldValue(const QString &FieldName, const QVariant &Value);

	QVariant GetNewValue(const QString &FieldName) const;
	void SetNewValue(const QString &FieldName, const QVariant &Value);

private:
	QMap<QString, ISObjectModelItem*> Items;
};
//-----------------------------------------------------------------------------
