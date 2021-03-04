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
	ISQueryLibPQ(const std::string &sql_text);
	ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text = std::string());
	~ISQueryLibPQ();

	const std::string& GetErrorString() const; //Получить текстовое описание ошибки
	const std::string& GetSqlText() const; //Получить текст запроса
	int GetResultSize() const; //Получить размер выборки
	bool GetIsSelect() const; //Проверить, является ли запрос выборкой

	void SetShowLongQuery(bool show_long_query);
	
	void First();
	bool Next();

	void BindValue(std::nullptr_t Pointer);
	void BindValue(int Value, Oid OID = InvalidOid);
	void BindValue(unsigned int Value, Oid OID = InvalidOid);
	void BindValue(const std::string &Value, Oid OID = InvalidOid);

	bool Execute(bool PrepareQuery = false, size_t ParamCount = 0);
	bool ExecuteFirst();

	Oid ColumnType(int Index); //Получить тип поля по индексу
	Oid ColumnType(const std::string &FieldName); //Получить тип поля по имени поля

	ISVariant ReadColumn(int Index);
	ISVariant ReadColumn(const std::string &FieldName);

private:
	void FillColumnMap(); //Заполнить мета-данные выборки
	bool Prepare(size_t ParamCount);

private:
	std::string ErrorString; //Тексовое описание ошибки
	bool ShowLongQuery; //Показывать долгие запрос
    std::string SqlText; //Текст запроса

	std::vector<std::string> ParameterValues; //Параметры запроса
	std::vector<Oid> ParameterTypes; //Типы параметров запроса
	size_t ParameterCount; //Количество параметров
	
	bool Prepared; //Флаг подготовленного запроса
	std::string StmtName; //Имя подготовленного оператора

	PGconn *SqlConnection; //Указатель на соединение с базой
	PGresult *SqlResult; //Структура с результатом запроса
	int CountRows; //Количество строк в результате запроса
	int CountColumns; //Количество полей в результате запроса
	int CurrentRow; //Индекс текущей строки результирующей выборки
	bool IsSelect; //Запрос является SELECT или SHOW

	std::map<int, ISSqlField> ColumnMap; //Мета-данные выборки
};
//-----------------------------------------------------------------------------
#endif
