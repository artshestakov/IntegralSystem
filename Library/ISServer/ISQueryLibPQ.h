#pragma once
#ifndef _ISQUERYLIBPQ_H_INCLUDED
#define _ISQUERYLIBPQ_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include <libpq-fe.h>
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISQueryLibPQ
{
public:
	ISQueryLibPQ(const QString &sql_text, bool prepare = false);
	ISQueryLibPQ(PGconn *sql_connection, const QString &sql_text = QString(), bool prepare = false);
	~ISQueryLibPQ();

	QString GetErrorString() const;
	int GetResultSize() const;
	
	void First();
	bool Next();

	bool Execute();

	bool ExecuteFirst();

	QString ReadColumn_String(const QString &FieldName);

private:
	bool Prepare();
	bool Prepare(PGconn *sql_connection);

private:
	QString ErrorString;
	bool ShowLongQuery;
	QString SqlText;
	PGconn *SqlConnection;
	PGresult *SqlResult;
	int CountRows;
	int CountColumns;
	int CurrentRow;
};
//-----------------------------------------------------------------------------
#endif
