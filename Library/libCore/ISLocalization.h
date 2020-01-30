#pragma once
#ifndef _ISLOCALIZATION_H_INCLUDED
#define _ISLOCALIZATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISLocalization
{
public:
	ISLocalization(const ISLocalization &) = delete;
	ISLocalization(ISLocalization &&) = delete;
	ISLocalization &operator=(const ISLocalization &) = delete;
	ISLocalization &operator=(ISLocalization &&) = delete;
	~ISLocalization();

	static ISLocalization& GetInstance();

	QString GetLocalString(const QString &ParameterName, const QString &SourceFile, int FileLine) const;
	void LoadResourceFile(const QString &FileName); //Инициализация файла из ресурсов
	void InitializeContent(const QString &Content); //Загрузка структуры локализации в буфер

private:
	ISLocalization();

	QStringMap Localization;
	QVectorString  LoadedFiles;
};
//-----------------------------------------------------------------------------
#define LOCALIZATION(PARAMETER) ISLocalization::GetInstance().GetLocalString(PARAMETER, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
