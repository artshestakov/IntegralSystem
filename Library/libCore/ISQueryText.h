#pragma once
#ifndef _ISQUERYTEXT_H_INCLUDED
#define _ISQUERYTEXT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQueryText
{
public:
	static ISQueryText& Instance();

	QString GetErrorString() const;
	QString InsertQuery(const char *QueryText, const char *FileName, int Line); //Добавить запрос в список
	bool CheckAllQueries(); //Проверка всех запросов

protected:
	void ErrorQuery(const ISSqlQuery &SqlQuery, const QString &ErrorQuery);

private:
	ISQueryText();
	~ISQueryText();
	ISQueryText(ISQueryText const &) {};
	ISQueryText& operator=(ISQueryText const&) { return *this; };

private:
	QString ErrorString;
	std::vector<ISSqlQuery> Vector;
};
//-----------------------------------------------------------------------------
#define PREPARE_QUERY(SqlText) ISQueryText::Instance().InsertQuery(SqlText, __FILE__, __LINE__)
#define PREPARE_QUERY2(SqlText) SqlText
//-----------------------------------------------------------------------------
#endif
