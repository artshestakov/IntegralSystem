#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISSqlModelView : public QAbstractItemModel
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSqlModelView(PMetaClassTable *meta_table, QObject *parent = 0);
	virtual ~ISSqlModelView();

	QSqlRecord GetRecord(int RowIndex) const; //Получить запись по идентификатору строки
	int GetFieldIndex(const QString &FieldName) const; //Получить индекс поля по его имени

	void Clear(); //Очистка модели
	void SetRecords(const QList<QSqlRecord> &records); //Изменить данные модели

	virtual QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
	virtual int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &Index) const override;

	void SetCurrentSorting(int IndexColumn, Qt::SortOrder Order);

protected:
	QIcon GetSortingIcon(int Section) const; //Получить иконку сортировки

private:
	PMetaClassTable *MetaTable;
	QList<QSqlRecord> Records; //Данные модели

	int CurrentSortingColumn;
	Qt::SortOrder CurrentColumnSortOrder;
	QIcon IconSortingUp;
	QIcon IconSortingDown;
};
//-----------------------------------------------------------------------------
