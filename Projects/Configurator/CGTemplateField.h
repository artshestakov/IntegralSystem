#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGTemplateField
{
public:
	static QString GetSqlTextForTemplateSystemFields(const QString &ClassName, const QString &ClassAlias); //Получить текст запроса для создания базовых полей класса
};
//-----------------------------------------------------------------------------
