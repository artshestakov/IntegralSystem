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

	QString GetErrorString() const;
	QString GetString(const QString &ParameterName); //Получить локализованную строку
	bool LoadResourceFile(const QString &FileName); //Инициализация файла из ресурсов
	bool LoadFile(const std::string &FilePath); //Инициализция файла с диска

private:
	bool InitializeContent(const QString &Content); //Загрузка структуры локализации в буфер

private:
	ISLocalization();
	~ISLocalization();
	ISLocalization(const ISLocalization&) = delete;
	ISLocalization(ISLocalization&&) = delete;
	ISLocalization& operator=(const ISLocalization&) = delete;
	ISLocalization& operator=(ISLocalization&&) = delete;

private:
	QString ErrorString;
	ISStringMap Dictionary;
	ISStringMapSTD DictionarySTD;
	ISVectorString LoadedFiles;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define LANG(PARAMETER) ISLocalization::Instance().GetString(PARAMETER)
//-----------------------------------------------------------------------------
#endif
