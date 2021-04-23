#pragma once
#ifndef _ISQUERY_H_INCLUDED
#define _ISQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
#include "ISDateTime.h"
#include "ISQueryText.h"
#include "libpq-fe.h"
#include "pg_type_d.h"
//-----------------------------------------------------------------------------
class ISQuery
{
public:
    ISQuery(PGconn *sql_connection, const std::string &sql_text = std::string());
    ISQuery(const std::string &sql_text);
    ~ISQuery();

    const std::string& GetErrorString() const; //Получить текстовое описание ошибки
    const std::string& GetSqlText() const; //Получить текст запроса
    int GetResultRowCount() const; //Получить размер выборки
    int GetResultColumnCount() const; //Получить кол-во полей в выборке
    Oid GetResultColumnType(size_t Index) const; //Получить тип поля в результирующей выборке
    const char* GetResultFieldName(int Index) const; //Получить имя поля из выборки
    bool GetIsSelect() const; //Проверить, является ли запрос выборкой

    void SetShowLongQuery(bool show_long_query);

    bool First();
    bool Next();

    void BindValue(std::nullptr_t Pointer);
    void BindValue(int Value, Oid OID = InvalidOid);
    void BindValue(unsigned int Value, Oid OID = InvalidOid);
    void BindValue(const std::string &Value, Oid OID = InvalidOid);
    void BindValue(const char *Value, Oid OID = InvalidOid);
    void BindValue(bool Value, Oid OID = InvalidOid);

    bool Execute();
    bool ExecuteFirst();

    bool IsNull(size_t Index) const;
    char* ReadColumn(size_t Index) const;
    std::string ReadColumn_String(size_t Index) const;
    int ReadColumn_Int(size_t Index) const;
    double ReadColumn_Double(size_t Index) const;
    unsigned int ReadColumn_UInt(size_t Index) const;
    ISInt64 ReadColumn_Int64(size_t Index) const;
    ISUInt64 ReadColumn_UInt64(size_t Index) const;
    bool ReadColumn_Bool(size_t Index) const;
    ISDate ReadColumn_Date(size_t Index) const;
    ISTime ReadColumn_Time(size_t Index) const;
    ISDateTime ReadColumn_DateTime(size_t Index) const;

private:
    bool Prepare(int ParamCount);
    void ClearResult();

private:
    std::string ErrorString; //Тексовое описание ошибки
    bool ShowLongQuery; //Показывать долгие запрос
    std::string SqlText; //Текст запроса

    std::vector<std::string> ParameterValues; //Параметры запроса
    std::vector<Oid> ParameterTypes; //Типы параметров запроса
    size_t ParameterCount; //Количество параметров

    std::string StmtName; //Имя подготовленного оператора

    PGconn *SqlConnection; //Указатель на соединение с базой
    PGresult *SqlResult; //Структура с результатом запроса
    int CountRows; //Количество строк в результате запроса
    int CountColumns; //Количество полей в результате запроса
    int CurrentRow; //Индекс текущей строки результирующей выборки
    bool IsSelect; //Запрос является SELECT или SHOW
};
//-----------------------------------------------------------------------------
#endif
