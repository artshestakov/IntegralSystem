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
    ISTcpQuery(const std::string &query_type);
    ~ISTcpQuery();

    const std::string& GetErrorString() const; //Получить текстовое описание ошибки
    void BindString(const std::string &ParamterName, const std::string &String); //Вставка параметра
    virtual bool Execute(const std::string &query_type); //Выполнить запрос по имени
    virtual bool Execute(); //Выполнить запрос
    //QVariantMap& GetAnswer(); //Получить ссылку на ответ
    //QVariantMap TakeAnswer(); //Вытащить ответ из класса

protected:
    //QVariant GetParameter(const QString &ParameterName) const; //Получить параметр по имени

private:
    bool IsValidAnswer(const std::string &StringBuffer, size_t Size, rapidjson::Value &ValueIsError, rapidjson::Value &ValueErrorString); //Проверка валидности ответа
    bool CheckConnection(); //Проверка соединения

private:
    std::string ErrorString; //Тестовое описание ошибки
    std::string QueryType; //Тип запроса
    rapidjson::Document Parameters; //Параметры запроса
    rapidjson::Value AnswerObject; //Ответ

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
