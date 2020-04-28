#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISSqlModelView : public QAbstractItemModel
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSqlModelView(PMetaTable *meta_table, QObject *parent = 0);
	virtual ~ISSqlModelView();

	QSqlRecord GetRecord(int RowIndex) const; //�������� ������ �� �������������� ������
	int GetFieldIndex(const QString &FieldName) const; //�������� ������ ���� �� ��� �����

	void Clear(); //������� ������
	void SetRecords(const std::vector<QSqlRecord> &records); //�������� ������ ������

	virtual QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
	virtual int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &Index) const override;

	void SetCurrentSorting(int IndexColumn, Qt::SortOrder Order);

protected:
	QIcon GetSortingIcon(int Section) const; //�������� ������ ����������

private:
	PMetaTable *MetaTable;
	std::vector<QSqlRecord> Records; //������ ������

	int CurrentSortingColumn;
	Qt::SortOrder CurrentColumnSortOrder;
	QIcon IconSortingUp;
	QIcon IconSortingDown;
};
//-----------------------------------------------------------------------------