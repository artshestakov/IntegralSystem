#pragma once
#ifndef _ISMETADATA_H_INCLUDED
#define _ISMETADATA_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaData
{
public:
    ISMetaData(const ISMetaData &) = delete;
    ISMetaData(ISMetaData &&) = delete;
    ISMetaData &operator=(const ISMetaData &) = delete;
    ISMetaData &operator=(ISMetaData &&) = delete;
    ~ISMetaData();

    static ISMetaData& Instance();

    QString GetErrorString() const;
    bool Initialize(const QString &configuration_name, bool InitXSR, bool InitXSF); //Инициализация

    PMetaTable* GetMetaTable(const QString &TableName); //Получить мета-таблицу по имени
    PMetaTable* GetMetaQuery(const QString &QueryName); //Получить мета-запрос по имени
    PMetaField* GetMetaField(PMetaTable *MetaTable, const QString &FieldName); //Получить мета-поле из указанной таблицы
    PMetaField* GetMetaField(const QString &TableName, const QString &FieldName); //Получить мета-поле из указанной таблицы

    std::vector<PMetaFunction*> GetFunctions(); //Получить список всех функций
    std::vector<PMetaTable*> GetTables(); //Получить список всех таблиц
    std::vector<PMetaIndex*> GetSystemIndexes(); //Получить список системных индексов
    std::vector<PMetaIndex*> GetIndexes(); //Получить список индексов для пользовательских полей
    std::vector<PMetaIndex*> GetCompoundIndexes(); //Получить список составных индексов
    std::vector<PMetaForeign*> GetForeigns(); //Получить список внешних ключей
    std::vector<PMetaResource*> GetResources(); //Получить ресурсы

    bool CheckExistTable(const QString &TableName) const; //Проверить наличие указанной таблицы в базе
    bool CheckExitField(PMetaTable *MetaTable, const QString &FieldName) const; //Проверить наличие указанного поля в указанной таблице
	bool CheckExistResource(const ISUuid &ResourceUID) const; //Проверить наличие ресурса по его идентификатору

    ISNamespace::FieldType GetTypeField(const QString &type_name); //Получить тип данных по мета-имени
    QString GetTypeDB(ISNamespace::FieldType field_type) const; //Получить наименование типа в базе данных
    QString GetTypeDB(const QString &type_name) const; //Получить наименование типа в базе данных по имени типа в системе
    QString GetNameFieldEdit(ISNamespace::FieldType field_type) const; //Получить наименование класса поля редактирования для типа данных
    QString GetSearchWidget(ISNamespace::FieldType field_type) const; //Получить имя поискового виджета
    bool GetSearch(ISNamespace::FieldType field_type) const; //Получить факт разрешение на поиск для типа данных

protected:
    bool CheckUniqueAllIdentifiers(bool InitXSR); //Проверка уникальности всех идентификаторов
    bool CheckUniqueAllAliases(); //Проверка уникальности всех псевдонимов таблиц
    bool GenerateSqlFromForeigns(); //Генерация SQL-запросов для внешних ключей

    bool InitializeXSN(); //Инициализация XSN
    bool InitializeXSN(const QString &Content); //Инициализация контента XSN

    bool InitializeXSNTable(QDomNode &DomNode); //Инициализация таблицы
    void InitializeXSNTableSystemFields(PMetaTable *MetaTable); //Инициализация системных полей для таблицы
    bool InitializeXSNTableSystemFieldsVisible(PMetaTable *MetaTable, const QDomNode &DomNode); //Инициализация видимости системных полей
    bool InitializeXSNTableFields(PMetaTable *MetaTable, const QDomNode &DomNode); //Инициализация полей таблицы
    bool InitializeXSNTableIndexes(PMetaTable *MetaTable, const QDomNode &DomNode); //Инициализация индексов полей таблицы
    bool InitializeXSNTableForeigns(PMetaTable *MetaTable, const QDomNode &DomNode); //Инициализация внешних ключей полей таблицы
    bool InitializeXSNTableEscorts(PMetaTable *MetaTable, const QDomNode &DomNode); //Инициализация эскортов таблицы
    bool InitializeXSNTableJoins(PMetaTable *MetaTable, const QDomNode &DomNode); //Инициализация JOIN'ов

    bool InitializeXSR(); //Инициализация XSR
    bool InitializeXSR(const QString &Content); //Инициализация файла XSR

    bool InitializeXSF(); //Иницилизация XSF
    bool InitializeXSF(const QString &Content); //Инициализация файла XSF

private:
    ISMetaData();

private:
    QString ErrorString;
    QString ConfigurationName;

    QDomNode GetChildDomNode(QDomNode &TableNode, const QString &TagName) const;

    std::map<QString, PMetaFunction *> FunctionsMap; //Функции
    std::map<QString, PMetaTable *> TablesMap; //Таблицы
    std::map<QString, PMetaTable *> QueriesMap; //Запросы

    std::vector<PMetaResource *> Resources; //Реурсы
    std::vector<PMetaIndex*> IndexesCompound; //Составные индексы

    QString CurrentXSN; //Текущий обрабатываемый XSN
    QString CurrentXSR; //Текущий обрабатываемый XSR
    QString CurrentXSF; //Текущий обрабатываемый XSF
    bool Initialized; //Флаг инициализации
    std::vector<ISMetaDataType> VectorTypes; //Перечисление типов системы
};
//-----------------------------------------------------------------------------
#endif
