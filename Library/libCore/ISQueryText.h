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
	ISQueryText(const ISQueryText &) = delete;
	ISQueryText(ISQueryText &&) = delete;
	ISQueryText &operator=(const ISQueryText &) = delete;
	ISQueryText &operator=(ISQueryText &&) = delete;
	~ISQueryText();

	static ISQueryText& GetInstance();
	QString GetErrorString() const;
	QString InsertQuery(const char *QueryText, const char *FileName, int Line); //Добавить запрос в список
	bool CheckAllQueries(); //Проверка всех запросов

protected:
	void ErrorQuery(ISSqlQuery SqlQuery, const QString &ErrorText);

private:
	ISQueryText();

	QString ErrorString;
	std::vector<ISSqlQuery> Vector;
};
//-----------------------------------------------------------------------------
#define PREPARE_QUERY(SqlText) ISQueryText::GetInstance().InsertQuery(SqlText, __FILE__, __LINE__);
//-----------------------------------------------------------------------------
#endif
