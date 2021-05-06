#pragma once
#ifndef _ISTCPQUERY_H_INCLUDED
#define _ISTCPQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include <rapidjson/document.h>
//-----------------------------------------------------------------------------
class ISTcpQuery
{
public:
    ISTcpQuery(const char *query_type);
    ~ISTcpQuery();

    const std::string& GetErrorString() const; //Получить текстовое описание ошибки
    void BindString(const std::string &ParamterName, const std::string &String); //Вставка параметра
    virtual bool Execute(const char *query_type); //Выполнить запрос по имени
    virtual bool Execute(); //Выполнить запрос
    const rapidjson::Value& GetAnswer() const; //Получить ссылку на ответ

protected:
    //QVariant GetParameter(const QString &ParameterName) const; //Получить параметр по имени

private:
    bool IsValidAnswer(const std::string &StringBuffer, size_t Size, bool &ValueIsError, std::string &ValueErrorString); //Проверка валидности ответа
    bool CheckConnection(); //Проверка соединения

private:
    std::string ErrorString; //Тестовое описание ошибки
    const char *QueryType; //Тип запроса
    rapidjson::Document Parameters; //Параметры запроса
    size_t ParametersCount; //Количество параметров
    rapidjson::Document AnswerObject; //Ответ

};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*class ISTcpQueryTable : public ISTcpQuery
{
public:
    ISTcpQueryTable();
    virtual ~ISTcpQueryTable();

    bool Execute() override;
    void SetSorting(const QString &SortingField, Qt::SortOrder SortingOrder); //Изменить сортировку
    void SetSearch(const QVariantList &VariantMap); //Изменяем поисковые условия
    void AddFilter(const QString &FieldName, const QVariant &Value); //Добавить фильтр

private:
    QVariantMap FilterMap;
    QVariantMap SortingMap;
};*/
//-----------------------------------------------------------------------------
#endif
