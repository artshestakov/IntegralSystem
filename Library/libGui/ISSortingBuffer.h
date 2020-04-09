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

	void AddSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting); //Добавить сортировку в память
	void SaveSortings(); //Сохранение сортировок в базу из памяти

	ISSortingMetaTable* GetSorting(const QString &TableName); //Получить мета-сортировку для таблицы

protected:
	void Initialize(); //Инициализация (чтение из базы)
	void SaveSorting(ISSortingMetaTable *MetaSorting); //Сохранение одной сортировки
	ISSortingMetaTable* CreateSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting); //Создание мета-сортировки

private:
	ISSortingBuffer();

	std::vector<ISSortingMetaTable*> Sortings;
};
//-----------------------------------------------------------------------------
