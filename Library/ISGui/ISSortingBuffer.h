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
	bool Initialize(); //Инициализация (чтение из базы)
	void AddSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting); //Добавить сортировку в память
	bool SaveSortings(); //Сохранение сортировок в базу из памяти
	bool Clear(); //Очистить сортировки

	ISSortingMetaTable* GetSorting(const QString &TableName); //Получить мета-сортировку для таблицы

private:
	bool SaveSorting(ISSortingMetaTable *MetaSorting); //Сохранение одной сортировки
	ISSortingMetaTable* CreateSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting); //Создание мета-сортировки

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
