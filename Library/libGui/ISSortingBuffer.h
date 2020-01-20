#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISSortingMetaTable.h"
//-----------------------------------------------------------------------------
class ISSortingBuffer : public QObject
{
	Q_OBJECT

public:
	ISSortingBuffer(const ISSortingBuffer &) = delete;
	ISSortingBuffer(ISSortingBuffer &&) = delete;
	ISSortingBuffer &operator=(const ISSortingBuffer &) = delete;
	ISSortingBuffer &operator=(ISSortingBuffer &&) = delete;
	~ISSortingBuffer();

	static ISSortingBuffer& GetInstance();

	void AddSorting(const QString &TableName, const QString &FieldName, int Sorting); //�������� ���������� � ������
	void SaveSortings(); //���������� ���������� � ���� �� ������

	ISSortingMetaTable* GetSorting(const QString &TableName); //�������� ����-���������� ��� �������

protected:
	void Initialize(); //������������� (������ �� ����)
	void SaveSorting(ISSortingMetaTable *MetaSorting); //���������� ����� ����������
	ISSortingMetaTable* CreateSorting(const QString &TableName, const QString &FieldName, int Sorting); //�������� ����-����������

private:
	ISSortingBuffer();

	QVector<ISSortingMetaTable*> Sortings;
};
//-----------------------------------------------------------------------------
