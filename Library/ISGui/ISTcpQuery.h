#pragma once
#ifndef _ISTCPQUERY_H_INCLUDED
#define _ISTCPQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISTcpQuery
{
public:
    ISTcpQuery(const QString &query_type);
    ~ISTcpQuery();

    QString GetErrorString() const; //Получить текстовое описание ошибки
    unsigned int GetVersion() const; //Получить версию протокола
    void BindValue(const QString &ParamterName, const QVariant &ParameterValue); //Вставка параметра
    virtual bool Execute(const QString &query_type); //Выполнить запрос по имени
    virtual bool Execute(); //Выполнить запрос
    QVariantMap& GetAnswer(); //Получить ссылку на ответ
    QVariantMap TakeAnswer(); //Вытащить ответ из класса

protected:
    QVariant GetParameter(const QString &ParameterName) const; //Получить параметр по имени

private:
    bool IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap); //Проверка валидности ответа

private:
    QString ErrorString; //Тестовое описание ошибки
    QString QueryType; //Тип запроса
    QVariantMap Parameters; //Параметры запроса
    QVariantMap TcpAnswer; //Ответ
    unsigned int Version; //Версия протокола
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTcpQueryTable : public ISTcpQuery
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
};
//-----------------------------------------------------------------------------
#endif
