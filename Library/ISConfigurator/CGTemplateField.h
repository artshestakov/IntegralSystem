#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGTemplateField : public QObject
{
	Q_OBJECT

public:
	CGTemplateField();
	virtual ~CGTemplateField();

	static QString GetSqlTextForTemplateSystemFields(const QString &ClassName, const QString &ClassAlias); //Получить текст запроса для создания базовых полей класса
};
//-----------------------------------------------------------------------------
