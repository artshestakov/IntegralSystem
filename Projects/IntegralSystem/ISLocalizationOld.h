#pragma once
#ifndef _ISLOCALIZATIONOLD_H_INCLUDED
#define _ISLOCALIZATIONOLD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefsOld.h"
//-----------------------------------------------------------------------------
class ISLocalizationOld
{
public:
	static ISLocalizationOld& Instance();

	QString GetErrorString() const;
	QString GetString(const QString &ParameterName); //Получить локализованную строку
	bool LoadResourceFile(const QString &FileName); //Инициализация файла из ресурсов

private:
	bool InitializeContent(const QString &Content); //Загрузка структуры локализации в буфер

private:
    ISLocalizationOld();
	~ISLocalizationOld();
	ISLocalizationOld(const ISLocalizationOld&) = delete;
	ISLocalizationOld(ISLocalizationOld&&) = delete;
	ISLocalizationOld& operator=(const ISLocalizationOld&) = delete;
	ISLocalizationOld& operator=(ISLocalizationOld&&) = delete;

private:
	QString ErrorString;
	ISStringMap Dictionary;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define LANG(PARAMETER) ISLocalizationOld::Instance().GetString(PARAMETER)
//-----------------------------------------------------------------------------
#endif
