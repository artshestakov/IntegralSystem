#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISColumnSizeItem
{
public:
	ISColumnSizeItem();
	~ISColumnSizeItem();

	void SetFieldSize(const QString &FieldName, int Size);
	int GetFieldSize(const QString &FieldName);

	void SetModificationFlag(bool modification_flag);
	bool GetModificationFlag() const;

	QMap<QString, int> GetFields();

private:
	QMap<QString, int> Fields;
	bool ModificationFlag;
};
//-----------------------------------------------------------------------------
