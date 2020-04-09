#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISSortingBuffer
{
public:
	ISSortingBuffer(const ISSortingBuffer &) = delete;
	ISSortingBuffer(ISSortingBuffer &&) = delete;
	ISSortingBuffer &operator=(const ISSortingBuffer &) = delete;
	ISSortingBuffer &operator=(ISSortingBuffer &&) = delete;
	~ISSortingBuffer();

	static ISSortingBuffer& GetInstance();

	void AddSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting); //�������� ���������� � ������
	void SaveSortings(); //���������� ���������� � ���� �� ������

	ISSortingMetaTable* GetSorting(const QString &TableName); //�������� ����-���������� ��� �������

protected:
	void Initialize(); //������������� (������ �� ����)
	void SaveSorting(ISSortingMetaTable *MetaSorting); //���������� ����� ����������
	ISSortingMetaTable* CreateSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting); //�������� ����-����������

private:
	ISSortingBuffer();

	std::vector<ISSortingMetaTable*> Sortings;
};
//-----------------------------------------------------------------------------
