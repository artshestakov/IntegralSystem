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
	bool LoadTraslatorQT(); //Загрузка трансляций QT
	bool LoadResourceFile(const QString &FileName); //Инициализация файла из ресурсов

private:
	bool InitializeContent(const QString &Content); //Загрузка структуры локализации в буфер

private:
	ISLocalization();
	~ISLocalization();
	ISLocalization(ISLocalization const &) {};
	ISLocalization& operator=(ISLocalization const&) { return *this; };

private:
	QString ErrorString;
	ISStringMap Dictionary;
	ISVectorString LoadedFiles;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define LANG(PARAMETER) ISLocalization::Instance().GetString(PARAMETER)
//-----------------------------------------------------------------------------
#endif
