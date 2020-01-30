#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISSqlModelCore : public QAbstractItemModel
{
	Q_OBJECT

public:
	ISSqlModelCore(PMetaClassTable *meta_table, QObject *parent = 0);
	virtual ~ISSqlModelCore();

	virtual void FillColumns(); //���������� �����
	void SetRecords(const QList<QSqlRecord> &records); //�������� ������ ������
	void Clear(); //�������� ������
	QSqlRecord GetRecord(int RowIndex) const; //�������� ������ �� �������������� ������
	void RemoveRecord(int Index); //������� ������ �� ������
	void RemoveColumn(PMetaClassField *MetaField); //������� ������� �� ������
	int GetFieldIndex(const QString &FieldName) const; //�������� ������ ���� �� ��� �����
	QString GetFieldLocalName(const QString &FieldName) const; //�������� ��������� ��� ����
	PMetaClassField* GetField(int Index); //�������� ����-���� �� ��� �������

	virtual QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role = Qt::EditRole) override;
	virtual QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
	virtual int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &Index) const override;

	void SetIsDeletedIndex(int IndexColumn);
	void SetIsSystemIndex(int IndexColumn);
	void SetCurrentSorting(int IndexColumn, Qt::SortOrder Order);
	void SetShowToolTip(bool show_tooltip);

protected:
	PMetaClassTable* GetMetaTable(); //�������� ����-�������
	void AppendField(PMetaClassField *MetaField); //�������� ����-���� � ������ ����� ������

	bool GetIsSystem(int RowIndex) const; //�������� ������ ��������� ������
	bool GetIsDeleted(int RowIndex) const; //�������� ������ ��������� ������
	QIcon GetSortingIcon(int Section) const; //�������� ������ ����������

private:
	PMetaClassTable *MetaTable; //����-�������
	QList<QSqlRecord> Records; //������ ������
	QVector<PMetaClassField*> Fields; //������ ����� �������

	int CurrentSortingColumn;
	Qt::SortOrder CurrentColumnSortOrder;
	int IsDeletedIndex;
	int IsSystemIndex;

	QIcon IconSortingUp;
	QIcon IconSortingDown;
	bool ShowToolTip; //��������� ������ ������������ ��������� ��� ������ (ToolTip)
};
//-----------------------------------------------------------------------------
