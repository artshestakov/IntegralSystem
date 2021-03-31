#pragma once
#ifndef _ISQUERY_H_INCLUDED
#define _ISQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
#include <libpq-fe.h>
#include <pg_type_d.h>
//-----------------------------------------------------------------------------
class ISQuery
{
public:
    ISQuery(PGconn *sql_connection, const std::string &sql_text = std::string());
    ISQuery(const std::string &sql_text);
    ~ISQuery();

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

    char* ReadColumn(size_t Index) const;
    std::string ReadColumn_String(size_t Index) const;
    int ReadColumn_Int(size_t Index) const;
    unsigned int ReadColumn_UInt(size_t Index) const;
    ISInt64 ReadColumn_Int64(size_t Index) const;
    ISUInt64 ReadColumn_UInt64(size_t Index) const;
    bool ReadColumn_Bool(size_t Index) const;

private:
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

    //std::map<size_t, ISSqlField> ColumnMap; //Мета-данные выборки
};
//-----------------------------------------------------------------------------
#endif
