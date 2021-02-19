#pragma once
#ifndef _ISQUERYLIBPQ_H_INCLUDED
#define _ISQUERYLIBPQ_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include <libpq-fe.h>
#include <pg_type_d.h>
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

	Oid ColumnType(int Index); //Получить тип поля по индексу
	Oid ColumnType(const std::string &FieldName); //Получить тип поля по имени поля

	char* ReadColumn(int Index);
	char* ReadColumn(const std::string &FieldName);

	std::string ReadColumn_String(int Index);
	std::string ReadColumn_String(const std::string &FieldName);

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
