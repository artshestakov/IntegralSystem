#pragma once
#ifndef _ISLOCALIZATION_H_INCLUDED
#define _ISLOCALIZATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLocalization
{
public:
	static ISLocalization& Instance();

	QString GetString(const QString &ParameterName) const; //Получить локализованную строку
	void LoadResourceFile(const QString &FileName); //Инициализация файла из ресурсов
	void InitializeContent(const QString &Content); //Загрузка структуры локализации в буфер

private:
	ISLocalization();
	~ISLocalization();
	ISLocalization(ISLocalization const &) {};
	ISLocalization& operator=(ISLocalization const&) { return *this; };

private:
	std::map<QString, QString> Dictionary;
	ISVectorString LoadedFiles;
};
//-----------------------------------------------------------------------------
#define LANG(PARAMETER) ISLocalization::Instance().GetString(PARAMETER)
//-----------------------------------------------------------------------------
#endif
