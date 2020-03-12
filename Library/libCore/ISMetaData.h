#pragma once
#ifndef _ISMETADATA_H_INCLUDED
#define _ISMETADATA_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "PMetaClass.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaData
{
public:
	ISMetaData(const ISMetaData &) = delete;
	ISMetaData(ISMetaData &&) = delete;
	ISMetaData &operator=(const ISMetaData &) = delete;
	ISMetaData &operator=(ISMetaData &&) = delete;
	~ISMetaData();

	static ISMetaData& GetInstanse();

	QString GetErrorString() const;
	bool Initialize(const QString &configuration_name, bool InitXSR, bool InitXSF); //Инициализация

	PMetaClassTable* GetMetaTable(const QString &TableName); //Получить мета-таблицу по имени
	PMetaClassTable* GetMetaQuery(const QString &QueryName); //Получить мета-запрос по имени
	PMetaClassField* GetMetaField(PMetaClassTable *MetaTable, const QString &FieldName); //Получить мета-поле из указанной таблицы
	PMetaClassField* GetMetaField(const QString &TableName, const QString &FieldName); //Получить мета-поле из указанной таблицы

	std::vector<PMetaClassFunction*> GetFunctions(); //Получить список всех функций
	std::vector<PMetaClassTable*> GetTables(); //Получить список всех таблиц
	ISVectorString GetMetaQueries(); //Получить вектор всех мета-запросов
	std::vector<PMetaClassIndex*> GetSystemIndexes(); //Получить список системных индексов
	std::vector<PMetaClassIndex*> GetIndexes(); //Получить список индексов для пользовательских полей
	std::vector<PMetaClassIndex*> GetCompoundIndexes(); //Получить список составных индексов
	std::vector<PMetaClassForeign*> GetForeigns(); //Получить список внешних ключей
	std::vector<PMetaClassResource*> GetResources(); //Получить ресурсы

	bool CheckExistTable(const QString &TableName) const; //Проверить наличие указанной таблицы в базе
	bool CheckExitField(PMetaClassTable *MetaTable, const QString &FieldName) const; //Проверить наличие указанного поля в указанной таблице

	ISNamespace::FieldType GetTypeField(const QString &type_name); //Получить тип данных по мета-имени
	QString GetTypeDB(ISNamespace::FieldType field_type) const; //Получить наименование типа в базе данных
	QString GetTypeDB(const QString &type_name) const; //Получить наименование типа в базе данных по имени типа в системе
	QString GetNameFieldEdit(ISNamespace::FieldType field_type) const; //Получить наименование класса для типа данных
	QString GetSearchWidget(ISNamespace::FieldType field_type) const; //Получить имя поискового виджета
	bool GetSearch(ISNamespace::FieldType field_type) const; //Получить факт разрешение на поиск для типа данных

protected:
	bool CheckUniqueAllIdentifiers(bool InitXSR); //Проверка уникальности всех идентификаторов
	bool CheckUniqueAllAliases(); //Проверка уникальности всех псевдонимов таблиц
	bool GenerateSqlFromForeigns(); //Генерация SQL-запросов для внешних ключей

	bool InitializeXSN(); //Инициализация XSN
	bool InitializeXSN(const QString &Content); //Инициализация контента XSN
	
	bool InitializeXSNTable(QDomNode &DomNode); //Инициализация таблицы
	void InitializeXSNTableSystemFields(PMetaClassTable *MetaTable); //Инициализация системных полей для таблицы
    bool InitializeXSNTableSystemFieldsVisible(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация видимости системных полей
    bool InitializeXSNTableFields(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация полей таблицы
    bool InitializeXSNTableIndexes(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация индексов полей таблицы
    bool InitializeXSNTableForeigns(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация внешних ключей полей таблицы
    bool InitializeXSNTableEscorts(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация эскортов таблицы
    bool InitializeXSNTableJoins(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация JOIN'ов

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

	std::map<QString, PMetaClassFunction *> FunctionsMap; //Функции
	std::map<QString, PMetaClassTable *> TablesMap; //Таблицы
	std::map<QString, PMetaClassTable *> QueriesMap; //Запросы

	std::vector<PMetaClassResource *> Resources; //Реурсы
	std::vector<PMetaClassIndex*> IndexesCompound; //Составные индексы

	QString CurrentXSN; //Текущий обрабатываемый XSN
	QString CurrentXSR; //Текущий обрабатываемый XSR
	QString CurrentXSF; //Текущий обрабатываемый XSF
	bool Initialized; //Флан инициализации
	std::vector<ISMetaDataType> VectorTypes; //Перечисление типов системы
};
//-----------------------------------------------------------------------------
#endif
