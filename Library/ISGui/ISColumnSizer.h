#pragma once
#ifndef _ISCOLUMNSIZER_H_INCLUDED
#define _ISCOLUMNSIZER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISColumnSizer
{
public:
	static ISColumnSizer& Instance();

	void Initialize(const QVariantMap &VariantMap);
	QVariantMap GetColumnSize() const; //�������� ������ � �������� �����

	void SetColumnSize(const QString &TableName, const QString &FieldName, int Size); //�������� ������ ���� � �������
	int GetColumnSize(const QString &TableName, const QString &FieldName) const; //�������� ������ ���� � �������

private:
	ISColumnSizer();
	~ISColumnSizer();
	ISColumnSizer(ISColumnSizer const &) {};
	ISColumnSizer& operator=(ISColumnSizer const&) { return *this; };

private:
	std::map<QString, ISStringToIntMap> Tables;
};
//-----------------------------------------------------------------------------
#endif
