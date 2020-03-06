#pragma once
#ifndef _ISCOLUMNSIZER_H_INCLUDED
#define _ISCOLUMNSIZER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISColumnSizer
{
public:
	static ISColumnSizer& Instance();

	bool Initialize();
	void Save(); //Сохранить размеры в базу данных
	void Clear(); //Очистить размеры

	void SetColumnSize(const QString &TableName, const QString &FieldName, int Size); //Изменить размер поля в таблице
	int GetColumnSize(const QString &TableName, const QString &FieldName) const; //Получить размер поля в таблице

private:
	ISColumnSizer();
	~ISColumnSizer();
	ISColumnSizer(ISColumnSizer const &) {};
	ISColumnSizer& operator=(ISColumnSizer const&) { return *this; };

private:
	QMap<QString, ISColumnSizeItem *> Tables;
};
//-----------------------------------------------------------------------------
#endif
