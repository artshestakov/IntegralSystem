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
    int GetErrorNumber() const; //Получить код ошибки
    
    const std::string& GetSqlText() const; //Получить текст запроса
    void SetSqlQuery(const std::string &sql_text); //Изменить текст запроса
    
    int GetResultRowCount() const; //Получить размер выборки
    int GetResultColumnCount() const; //Получить кол-во полей в выборке
    Oid GetResultColumnType(size_t Index) const; //Получить тип поля в результирующей выборке
    const char* GetResultFieldName(int Index) const; //Получить имя поля из выборки
    size_t GetResultAffected() const; //Получить количество затронтых строк
    bool GetIsSelect() const; //Проверить, является ли запрос выборкой

    void SetShowLongQuery(bool show_long_query);

    bool First();
    bool Next();

    void Bind(const std::string &String);
    void BindNull();
    void BindString(const std::string &String);
    void BindString(const char *String);
    void BindUID(const std::string &UID);
    void BindUID(const char *UID);
    void BindBool(bool Value);
    void BindBinary(unsigned char *Data, size_t Size);
    void BindUInt(unsigned int Value);
    void BindUInt64(uint64_t Value);
    void BindInt(int Value);
    void BindInt64(int64_t Value);
    void BindDouble(double Value);
    void BindDate(const ISDate &Date);

    bool Execute();
    bool ExecuteFirst();

    bool IsNull(size_t Index) const;
    char* ReadColumn(size_t Index) const;
    std::string ReadColumn_String(size_t Index) const;
    int ReadColumn_Int(size_t Index) const;
    double ReadColumn_Double(size_t Index) const;
    unsigned int ReadColumn_UInt(size_t Index) const;
    uint64_t ReadColumn_Int64(size_t Index) const;
    uint64_t ReadColumn_UInt64(size_t Index) const;
    bool ReadColumn_Bool(size_t Index) const;
    ISDate ReadColumn_Date(size_t Index) const;
    ISTime ReadColumn_Time(size_t Index) const;
    ISDateTime ReadColumn_DateTime(size_t Index) const;
    unsigned char* ReadColumn_Binary(size_t Index) const;
    unsigned char* ReadColumn_Binary(size_t Index, size_t &DataSize) const;

private:
    bool Prepare(int ParamCount);
    void ClearResult();

private:
    std::string ErrorString; //Тексовое описание ошибки
    int ErrorNumber; //Код ошибки
    char *ErrorSeverity; //Серьёзность ошибки
    char *ErrorPrimary; //Главное описание ошибки
    char *ErrorDetail; //Детали ошибки
    char *ErrorHint; //Подсказка по устранению ошибки

    bool ShowLongQuery; //Показывать долгие запрос
    std::string SqlText; //Текст запроса

    std::vector<char *> ParameterValues; //Параметры запроса
    std::vector<Oid> ParameterTypes; //Типы параметров запроса
    std::vector<int> ParameterFormats; //Форматы параметров
    std::vector<int> ParameterLengths;
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
