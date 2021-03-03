#pragma once
#ifndef _ISQUERYLIBPQ_H_INCLUDED
#define _ISQUERYLIBPQ_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include "ISStructs.h"
#include "ISVariant.h"
#include <libpq-fe.h>
#include <pg_type_d.h>
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISQueryLibPQ
{
public:
	ISQueryLibPQ(const std::string &sql_text, bool prepare = false, int ParamCount = 0);
	ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text = std::string(), bool prepare = false, int ParamCount = 0);
	~ISQueryLibPQ();

	const std::string& GetErrorString() const; //Получить текстовое описание ошибки
	const std::string& GetSqlText() const; //Получить текст запроса
	int GetResultSize() const; //Получить размер выборки
	bool GetIsSelect() const; //Проверить, является ли запрос выборкой

	void SetShowLongQuery(bool show_long_query);
	
	void First();
	bool Next();

	void AddBindValue(const ISVariant &Value, Oid OID = NULL);

	bool Execute();

	bool ExecuteFirst();

	Oid ColumnType(int Index); //Получить тип поля по индексу
	Oid ColumnType(const std::string &FieldName); //Получить тип поля по имени поля

	ISVariant ReadColumn(int Index);
	ISVariant ReadColumn(const std::string &FieldName);

private:
	void FillColumnMap();
	bool Prepare(int ParamCount);
	bool Prepare(PGconn *sql_connection, int ParamCount);

private:
	std::string ErrorString; //Тексовое описание ошибки
	bool ShowLongQuery; //Показывать долгие запрос
    std::string SqlText; //Текст запроса
	std::vector<ISVariant> Parameters; //Параметры запроса
	std::vector<Oid> Types; //Типы параметров запроса
	size_t ParametersCount; //Количество параметров
	std::string StmtName; //Имя подготовленного оператора
	bool Prepared; //Флаг подготовки запроса
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
