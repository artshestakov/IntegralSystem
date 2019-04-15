#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISQueryText : public QObject
{
	Q_OBJECT

public:
	ISQueryText(const ISQueryText &) = delete;
	ISQueryText(ISQueryText &&) = delete;
	ISQueryText &operator=(const ISQueryText &) = delete;
	ISQueryText &operator=(ISQueryText &&) = delete;
	~ISQueryText();

	static ISQueryText& GetInstance();
	
	QString InsertQuery(const char *QueryText, const char *FileName, int Line); //Добавить запрос в список
	void CheckAllQueries(); //Проверка всех запросов

protected:
	void ErrorQuery(const QString &About, const QString &SqlText, const QString &ErrorText);

private:
	ISQueryText();

	QStringMap Queries;
};
//-----------------------------------------------------------------------------
#define PREPARE_QUERY(SqlText) ISQueryText::GetInstance().InsertQuery(SqlText, __FILE__, __LINE__);
//-----------------------------------------------------------------------------
