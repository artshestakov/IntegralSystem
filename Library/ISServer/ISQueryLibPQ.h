#pragma once
#ifndef _ISQUERYLIBPQ_H_INCLUDED
#define _ISQUERYLIBPQ_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include "ISStructs.h"
#include "ISVariant.h"
#include <libpq-fe.h>
#include <pg_type_d.h>
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISQueryLibPQ
{
public:
	ISQueryLibPQ(const std::string &sql_text, bool prepare = false, int ParamCount = 0);
	ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text = std::string(), bool prepare = false, int ParamCount = 0);
	~ISQueryLibPQ();

	const std::string& GetErrorString() const; //�������� ��������� �������� ������
	const std::string& GetSqlText() const; //�������� ����� �������
	int GetResultSize() const; //�������� ������ �������
	bool GetIsSelect() const; //���������, �������� �� ������ ��������

	void SetShowLongQuery(bool show_long_query);
	
	void First();
	bool Next();

	void AddBindValue(const ISVariant &Value, Oid OID = NULL);

	bool Execute();

	bool ExecuteFirst();

	Oid ColumnType(int Index); //�������� ��� ���� �� �������
	Oid ColumnType(const std::string &FieldName); //�������� ��� ���� �� ����� ����

	ISVariant ReadColumn(int Index);
	ISVariant ReadColumn(const std::string &FieldName);

private:
	void FillColumnMap();
	bool Prepare(int ParamCount);
	bool Prepare(PGconn *sql_connection, int ParamCount);

private:
	std::string ErrorString; //�������� �������� ������
	bool ShowLongQuery; //���������� ������ ������
    std::string SqlText; //����� �������
	std::vector<ISVariant> Parameters; //��������� �������
	std::vector<Oid> Types; //���� ���������� �������
	size_t ParametersCount; //���������� ����������
	std::string StmtName; //��� ��������������� ���������
	bool Prepared; //���� ���������� �������
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
