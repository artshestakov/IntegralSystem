#pragma once
#ifndef _CGCONFIGURATORFIAS_H_INCLUDED
#define _CGCONFIGURATORFIAS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGConfiguratorFIAS : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorFIAS();
	virtual ~CGConfiguratorFIAS();

public slots:
	void prepare(); //Подготовка выгрузки
	void update(); //Обновление

private:

	//Подготовка
	bool FileHandling(const QFileInfo &FileInfo, const QString &ResultPath); //Обработка файла выгрузки
	QString GetFileResultName(const QString &FileName) const; //Получить наименование файла с результатом обработки

	//Обновление
	void InitializeKeys();
	bool FileUpload(const QFileInfo &FileInfo); //Загрузка файла в базу
	quint64 GetCountLine(const QString &FilePath) const; //Получить количество строк в файле
	QString GetTableName(const QString &FilePath) const; //Получить наименование таблицы
	ISStringMap ParseLine(const QString &Content) const; //Парсинг строки
	bool Select(PMetaClassTable *MetaTable, const ISStringMap &StringMap) const;
	void Update(PMetaClassTable *MetaTable, const ISStringMap &StringMap);
	void Insert(PMetaClassTable *MetaTable, const ISStringMap &StringMap);

private:
	ISStringMap MapKeys;
};
//-----------------------------------------------------------------------------
#endif
