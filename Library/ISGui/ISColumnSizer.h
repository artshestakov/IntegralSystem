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

	QString GetErrorString() const; //Получить описание ошибки
	bool Initialize();
	bool Save(); //Сохранить размеры в базу данных
	bool Clear(); //Очистить размеры

	void SetColumnSize(const QString &TableName, const QString &FieldName, int Size); //Изменить размер поля в таблице
	int GetColumnSize(const QString &TableName, const QString &FieldName) const; //Получить размер поля в таблице

private:
	ISColumnSizer();
	~ISColumnSizer();
	ISColumnSizer(ISColumnSizer const &) {};
	ISColumnSizer& operator=(ISColumnSizer const&) { return *this; };

private:
	QString ErrorString;
	std::map<QString, ISColumnSizeItem*> Tables;
	std::map<QString, ISStringToIntMap> TablesNew;
};
//-----------------------------------------------------------------------------
#endif
