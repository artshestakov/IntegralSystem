#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISSortingBuffer
{
public:
	static ISSortingBuffer& Instance();

	QString GetErrorString() const;
	bool Initialize(); //������������� (������ �� ����)
	void AddSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting); //�������� ���������� � ������
	bool SaveSortings(); //���������� ���������� � ���� �� ������
	bool Clear(); //�������� ����������

	ISSortingMetaTable* GetSorting(const QString &TableName); //�������� ����-���������� ��� �������

private:
	bool SaveSorting(ISSortingMetaTable *MetaSorting); //���������� ����� ����������
	ISSortingMetaTable* CreateSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting); //�������� ����-����������

private:
	ISSortingBuffer();
	~ISSortingBuffer();
	ISSortingBuffer(ISSortingBuffer const &) {};
	ISSortingBuffer& operator=(ISSortingBuffer const&) { return *this; };

private:
	QString ErrorString;
	std::vector<ISSortingMetaTable*> Sortings;
};
//-----------------------------------------------------------------------------
