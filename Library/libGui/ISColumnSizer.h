#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISColumnSizeItem.h"
//-----------------------------------------------------------------------------
class ISColumnSizer : public QObject
{
	Q_OBJECT

public:
	ISColumnSizer(const ISColumnSizer &) = delete;
	ISColumnSizer(ISColumnSizer &&) = delete;
	ISColumnSizer &operator=(const ISColumnSizer &) = delete;
	ISColumnSizer &operator=(ISColumnSizer &&) = delete;
	~ISColumnSizer();

	static ISColumnSizer& GetInstance();

	void Initialize();
	void Save(); //��������� ������� � ���� ������
	void Clear(); //�������� �������

	void SetColumnSize(const QString &TableName, const QString &FieldName, int Size); //�������� ������ ���� � �������
	int GetColumnSize(const QString &TableName, const QString &FieldName) const; //�������� ������ ���� � �������

private:
	ISColumnSizer();

	QMap<QString, ISColumnSizeItem*> Tables;
};
//-----------------------------------------------------------------------------
