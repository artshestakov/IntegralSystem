#pragma once
#ifndef _ISMETADATA_H_INCLUDED
#define _ISMETADATA_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "PMetaClassFunction.h"
#include "PMetaClassTable.h"
#include "PMetaClassQuery.h"
#include "PMetaClassResource.h"
#include "ISAssociationTypes.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaData : public QObject
{
	Q_OBJECT

public:
	ISMetaData(const ISMetaData &) = delete;
	ISMetaData(ISMetaData &&) = delete;
	ISMetaData &operator=(const ISMetaData &) = delete;
	ISMetaData &operator=(ISMetaData &&) = delete;
	~ISMetaData();

	static ISMetaData& GetInstanse();

	bool GetInitialized() const;
	void Initialize(const QString &configuration, bool InitXSR, bool InitXSF); //Инициализация

	PMetaClassTable* GetMetaTable(const QString &TableName); //Получить мета-таблицу по имени
	PMetaClassTable* GetMetaQuery(const QString &QueryName); //Получить мета-запрос по имени
	PMetaClassField* GetMetaField(PMetaClassTable *MetaTable, const QString &FieldName); //Получить мета-поле из указанной таблицы
	PMetaClassField* GetMetaField(const QString &TableName, const QString &FieldName); //Получить мета-поле из указанной таблицы

	QList<PMetaClassFunction*> GetFunctions(); //Получить список всех функций
	QList<PMetaClassTable*> GetTables(); //Получить список всех таблиц
	QVectorString GetMetaQueries(); //Получить вектор всех мета-запросов
	QVector<PMetaClassIndex*> GetSystemIndexes(); //Получить список системных индексов
	QVector<PMetaClassIndex*> GetIndexes(); //Получить список индексов для пользовательских полей
	QVector<PMetaClassIndex*> GetCompoundIndexes(); //Получить список составных индексов
	QVector<PMetaClassForeign*> GetForeigns(); //Получить список внешних ключей
	QVector<PMetaClassResource*> GetResources(); //Получить ресурсы

	bool CheckExistTable(const QString &TableName) const; //Проверить наличие указанной таблицы в базе
	bool CheckExitField(PMetaClassTable *MetaTable, const QString &FieldName) const; //Проверить наличие указанного поля в указанной таблице
	ISAssociationTypes& GetAssociationTypes(); //Получить ассоциации

protected:
	void CheckUniqueAllIdentifiers(bool InitXSR); //Проверка уникальности всех идентификаторов
	void CheckUniqueAllAliases(); //Проверка уникальности всех псевдонимов таблиц
	void GenerateSqlFromForeigns(); //Генерация SQL-запросов для внешних ключей

	void InitializeXSN(); //Инициализация XSN
	void InitializeXSN(const QString &Content); //Инициализация контента XSN
	
	void InitializeXSNTable(QDomNode &DomNode); //Инициализация таблицы
	void InitializeXSNTableSystemFields(PMetaClassTable *MetaTable); //Инициализация системных полей для таблицы
    void InitializeXSNTableSystemFieldsVisible(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация видимости системных полей
    void InitializeXSNTableFields(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация полей таблицы
    void InitializeXSNTableIndexes(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация индексов полей таблицы
    void InitializeXSNTableForeigns(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация внешних ключей полей таблицы
    void InitializeXSNTableEscorts(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация эскортов таблицы
    void InitializeXSNTableJoins(PMetaClassTable *MetaTable, const QDomNode &DomNode); //Инициализация JOIN'ов

	void InitializeXSR(); //Инициализация XSR
	void InitializeXSR(const QString &Content); //Инициализация файла XSR

	void InitializeXSF(); //Иницилизация XSF
	void InitializeXSF(const QString &Content); //Инициализация файла XSF

private:
	ISMetaData();
	QString Configuration;

    QDomNode GetChildDomNode(QDomNode &TableNode, const QString &TagName) const;

	QMap<QString, PMetaClassFunction*> FunctionsMap; //Функции
	QMap<QString, PMetaClassTable*> TablesMap; //Таблицы
	QMap<QString, PMetaClassTable*> QueriesMap; //Запросы

	QVector<PMetaClassResource*> Resources; //Реурсы
	QVector<PMetaClassIndex*> IndexesCompound; //Составные индексы

	QString CurrentXSN; //Текущий обрабатываемый XSN
	QString CurrentXSR; //Текущий обрабатываемый XSR
	QString CurrentXSF; //Текущий обрабатываемый XSF
	bool Initialized;

	ISAssociationTypes AssociationTypes;
};
//-----------------------------------------------------------------------------
#endif
