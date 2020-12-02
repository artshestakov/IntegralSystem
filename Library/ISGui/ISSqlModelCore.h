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

	void SetRecords(const std::vector<QSqlRecord> &records, const ISVectorString &fields); //Изменить данные модели
	void Clear(); //Очистить модель
	QSqlRecord GetRecord(int RowIndex) const; //Получить запись по идентификатору строки
	void RemoveRecord(int Index); //Удалить запись из модели
	void RemoveColumn(PMetaField *MetaField); //Удалить столбец из модели
	int GetFieldIndex(const QString &FieldName) const; //Получить индекс поля по его имени
	QString GetFieldLocalName(const QString &FieldName) const; //Получить локальное имя поля
	PMetaField* GetField(int Index); //Получить мета-поле по его индексу
	int GetSortColumn() const; //Получить индекс сортировки текущего поля
	Qt::SortOrder GetSortOrder() const; //Получить текущий порядок сортировки
	
	template<typename T> T GetFieldSum(const QString &FieldName, T InitValue) //Получить массив значений из поля по его имени
	{
		return GetFieldSum<T>(GetFieldIndex(FieldName), InitValue);
	}

	template<typename T> T GetFieldSum(int FieldIndex, T InitValue) //Получить массив значений из поля по его индексу
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
	PMetaTable* GetMetaTable(); //Получить мета-таблицу

private:
	PMetaTable *MetaTable; //Мета-таблица
	std::vector<QSqlRecord> Records; //Данные модели
	std::vector<PMetaField*> Fields; //Список полей таблицы

	int SortingColumn;
	Qt::SortOrder SortingOrder;
	int IsSystemIndex;

	QIcon IconSortingUp;
	QIcon IconSortingDown;
	bool ShowToolTip; //Индикатор показа всплывающего сообщения для ячейки (ToolTip)
};
//-----------------------------------------------------------------------------
