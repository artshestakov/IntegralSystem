#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISSqlModelCore : public QAbstractItemModel
{
	Q_OBJECT

public:
	ISSqlModelCore(PMetaTable *meta_table, QObject *parent = 0);
	virtual ~ISSqlModelCore();

	void SetRecords(const std::vector<QSqlRecord> &records, const ISVectorString &fields); //�������� ������ ������
	void Clear(); //�������� ������
	QSqlRecord GetRecord(int RowIndex) const; //�������� ������ �� �������������� ������
	void RemoveRecord(int Index); //������� ������ �� ������
	void RemoveColumn(PMetaField *MetaField); //������� ������� �� ������
	int GetFieldIndex(const QString &FieldName) const; //�������� ������ ���� �� ��� �����
	QString GetFieldLocalName(const QString &FieldName) const; //�������� ��������� ��� ����
	PMetaField* GetField(int Index); //�������� ����-���� �� ��� �������
	int GetSortColumn() const; //�������� ������ ���������� �������� ����
	Qt::SortOrder GetSortOrder() const; //�������� ������� ������� ����������
	
	template<typename T> T GetFieldSum(const QString &FieldName, T InitValue) //�������� ������ �������� �� ���� �� ��� �����
	{
		return GetFieldSum<T>(GetFieldIndex(FieldName), InitValue);
	}

	template<typename T> T GetFieldSum(int FieldIndex, T InitValue) //�������� ������ �������� �� ���� �� ��� �������
	{
		T Result = InitValue;
		for (int i = 0, c = rowCount(); i < c; ++i)
		{
			Result += data(index(i, FieldIndex)).value<T>();
		}
		return Result;
	}

	virtual QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role = Qt::EditRole) override;
	virtual QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
	virtual int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &Index) const override;

	void SetSorting(int IndexColumn, Qt::SortOrder Order);
	void SetShowToolTip(bool show_tool_tip);

protected:
	PMetaTable* GetMetaTable(); //�������� ����-�������

private:
	PMetaTable *MetaTable; //����-�������
	std::vector<QSqlRecord> Records; //������ ������
	std::vector<PMetaField*> Fields; //������ ����� �������

	int SortingColumn;
	Qt::SortOrder SortingOrder;
	int IsSystemIndex;

	QIcon IconSortingUp;
	QIcon IconSortingDown;
	bool ShowToolTip; //��������� ������ ������������ ��������� ��� ������ (ToolTip)
};
//-----------------------------------------------------------------------------
