#pragma once
#ifndef _ISQUERYLIBPQ_H_INCLUDED
#define _ISQUERYLIBPQ_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include "ISStructs.h"
#include <libpq-fe.h>
#include <pg_type_d.h>
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISQueryLibPQ
{
public:
	ISQueryLibPQ(const std::string &sql_text, bool prepare = false);
	ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text = std::string(), bool prepare = false);
	~ISQueryLibPQ();

	std::string GetErrorString() const; //�������� ��������� �������� ������
	int GetResultSize() const; //�������� ������ �������
	bool GetIsSelect() const; //���������, �������� �� ������ ��������

	void SetShowLongQuery(bool show_long_query);
	
	void First();
	bool Next();

	void AddBindValue(const char *Value);

	bool Execute();

	bool ExecuteFirst();

	Oid ColumnType(int Index); //�������� ��� ���� �� �������
	Oid ColumnType(const std::string &FieldName); //�������� ��� ���� �� ����� ����

	char* ReadColumn(int Index);
	char* ReadColumn(const std::string &FieldName);

	std::string ReadColumn_String(int Index);
	std::string ReadColumn_String(const std::string &FieldName);

private:
	void FillColumnMap();
	bool Prepare();
	bool Prepare(PGconn *sql_connection);

private:
	std::string ErrorString; //�������� �������� ������
	bool ShowLongQuery; //���������� ������ ������
	std::vector<const char *> Parameters;
	std::string SqlText; //����� �������
	PGconn *SqlConnection; //��������� �� ���������� � �����
	PGresult *SqlResult; //��������� � ����������� �������
	int CountRows; //���������� ����� � ���������� �������
	int CountColumns; //���������� ����� � ���������� �������
	int CurrentRow; //������ ������� ������ �������������� �������
	bool IsSelect; //������ �������� SELECT ��� SHOW
	std::map<int, ISSqlField> ColumnMap;
};
//-----------------------------------------------------------------------------
#endif