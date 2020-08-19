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

	virtual void FillColumns(); //Заполнение полей
	void SetRecords(const std::vector<QSqlRecord> &records); //Изменить данные модели
	void Clear(); //Очистить модель
	QSqlRecord GetRecord(int RowIndex) const; //Получить запись по идентификатору строки
	void RemoveRecord(int Index); //Удалить запись из модели
	void RemoveColumn(PMetaField *MetaField); //Удалить столбец из модели
	int GetFieldIndex(const QString &FieldName) const; //Получить индекс поля по его имени
	QString GetFieldLocalName(const QString &FieldName) const; //Получить локальное имя поля
	PMetaField* GetField(int Index); //Получить мета-поле по его индексу
	int GetSortColumn() const; //Получить индекс сортировки текущего поля
	Qt::SortOrder GetSortOrder() const; //Получить текущий порядок сортировки

	virtual QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role = Qt::EditRole) override;
	virtual QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
	virtual int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &Index) const override;

	void SetIsDeletedIndex(int IndexColumn);
	void SetIsSystemIndex(int IndexColumn);
	void SetSorting(const QString &FieldName, Qt::SortOrder Order);
	void SetSorting(int IndexColumn, Qt::SortOrder Order);
	void SetShowToolTip(bool show_tool_tip);

protected:
	PMetaTable* GetMetaTable(); //Получить мета-таблицу
	void AppendField(PMetaField *MetaField); //Добавить мета-поле в список полей модели

	bool GetIsSystem(int RowIndex) const; //Получить статус системной записи
	bool GetIsDeleted(int RowIndex) const; //Получить статус удаленной записи
	QIcon GetSortingIcon(int Section) const; //Получить иконку сортировки

private:
	PMetaTable *MetaTable; //Мета-таблица
	std::vector<QSqlRecord> Records; //Данные модели
	std::vector<PMetaField*> Fields; //Список полей таблицы

	int SortingColumn;
	Qt::SortOrder SortingOrder;
	int IsDeletedIndex;
	int IsSystemIndex;

	QIcon IconSortingUp;
	QIcon IconSortingDown;
	bool ShowToolTip; //Индикатор показа всплывающего сообщения для ячейки (ToolTip)
};
//-----------------------------------------------------------------------------
