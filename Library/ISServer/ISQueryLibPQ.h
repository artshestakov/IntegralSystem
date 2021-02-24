#pragma once
#ifndef _ISQUERYLIBPQ_H_INCLUDED
#define _ISQUERYLIBPQ_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include "ISStructs.h"
#include <libpq-fe.h>
#include <pg_type_d.h>
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISQueryLibPQ
{
public:
	ISQueryLibPQ(const std::string &sql_text, bool prepare = false, size_t ParamCount = 0);
	ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text = std::string(), bool prepare = false, size_t ParamCount = 0);
	~ISQueryLibPQ();

	std::string GetErrorString() const; //Получить текстовое описание ошибки
	int GetResultSize() const; //Получить размер выборки
	bool GetIsSelect() const; //Проверить, является ли запрос выборкой

	void SetShowLongQuery(bool show_long_query);
	
	void First();
	bool Next();

	void AddBindValue(char *Value);

	bool Execute();

	bool ExecuteFirst();

	Oid ColumnType(int Index); //Получить тип поля по индексу
	Oid ColumnType(const std::string &FieldName); //Получить тип поля по имени поля

	char* ReadColumn(int Index);
	char* ReadColumn(const std::string &FieldName);

	std::string ReadColumn_String(int Index);
	std::string ReadColumn_String(const std::string &FieldName);

private:
	void FillColumnMap();
	bool Prepare(size_t ParamCount);
	bool Prepare(PGconn *sql_connection, size_t ParamCount);

private:
	std::string ErrorString; //Тексовое описание ошибки
	bool ShowLongQuery; //Показывать долгие запрос
	std::vector<char *> Parameters;
	size_t ParametersCount; //Количество параметров
	std::string StmtName; //Имя подготовленного оператора
	bool Prepared; //Флаг подготовки запроса
	std::string SqlText; //Текст запроса
	PGconn *SqlConnection; //Указатель на соединение с базой
	PGresult *SqlResult; //Структура с результатом запроса
	int CountRows; //Количество строк в результате запроса
	int CountColumns; //Количество полей в результате запроса
	int CurrentRow; //Индекс текущей строки результирующей выборки
	bool IsSelect; //Запрос является SELECT или SHOW
	std::map<int, ISSqlField> ColumnMap;
};
//-----------------------------------------------------------------------------
#endif
