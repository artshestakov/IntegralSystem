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
	ISQueryLibPQ(const std::string &sql_text, bool prepare = false, size_t ParamCount = 0);
	ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text = std::string(), bool prepare = false, size_t ParamCount = 0);
	~ISQueryLibPQ();

	std::string GetErrorString() const; //�������� ��������� �������� ������
	int GetResultSize() const; //�������� ������ �������
	bool GetIsSelect() const; //���������, �������� �� ������ ��������

	void SetShowLongQuery(bool show_long_query);
	
	void First();
	bool Next();

	void AddBindValue(char *Value);

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
	bool Prepare(size_t ParamCount);
	bool Prepare(PGconn *sql_connection, size_t ParamCount);

private:
	std::string ErrorString; //�������� �������� ������
	bool ShowLongQuery; //���������� ������ ������
	std::vector<char *> Parameters;
	size_t ParametersCount; //���������� ����������
	std::string StmtName; //��� ��������������� ���������
	bool Prepared; //���� ���������� �������
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
