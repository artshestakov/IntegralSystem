#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISTypes.h"
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
	
	QString InsertQuery(const char *QueryText, const char *FileName, int Line); //�������� ������ � ������
	void CheckAllQueries(); //�������� ���� ��������

protected:
	void ErrorQuery(const QString &About, const QString &SqlText, const QString &ErrorText);

private:
	ISQueryText();

	QStringMap Queries;
};
//-----------------------------------------------------------------------------
#define PREPARE_QUERY(SqlText) ISQueryText::GetInstance().InsertQuery(SqlText, __FILE__, __LINE__);
//-----------------------------------------------------------------------------
