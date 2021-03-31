#pragma once
#ifndef _ISQUERY_H_INCLUDED
#define _ISQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
#include <libpq-fe.h>
#include <pg_type_d.h>
//-----------------------------------------------------------------------------
class ISQuery
{
public:
    ISQuery(PGconn *sql_connection, const std::string &sql_text = std::string());
    ISQuery(const std::string &sql_text);
    ~ISQuery();

    const std::string& GetErrorString() const; //�������� ��������� �������� ������
    const std::string& GetSqlText() const; //�������� ����� �������
    int GetResultSize() const; //�������� ������ �������
    bool GetIsSelect() const; //���������, �������� �� ������ ��������

    void SetShowLongQuery(bool show_long_query);

    void First();
    bool Next();

    void BindValue(std::nullptr_t Pointer);
    void BindValue(int Value, Oid OID = InvalidOid);
    void BindValue(unsigned int Value, Oid OID = InvalidOid);
    void BindValue(const std::string &Value, Oid OID = InvalidOid);

    bool Execute(bool PrepareQuery = false, size_t ParamCount = 0);
    bool ExecuteFirst();

    char* ReadColumn(size_t Index) const;
    std::string ReadColumn_String(size_t Index) const;
    int ReadColumn_Int(size_t Index) const;
    unsigned int ReadColumn_UInt(size_t Index) const;
    ISInt64 ReadColumn_Int64(size_t Index) const;
    ISUInt64 ReadColumn_UInt64(size_t Index) const;
    bool ReadColumn_Bool(size_t Index) const;

private:
    bool Prepare(size_t ParamCount);

private:
    std::string ErrorString; //�������� �������� ������
    bool ShowLongQuery; //���������� ������ ������
    std::string SqlText; //����� �������

    std::vector<std::string> ParameterValues; //��������� �������
    std::vector<Oid> ParameterTypes; //���� ���������� �������
    size_t ParameterCount; //���������� ����������

    bool Prepared; //���� ��������������� �������
    std::string StmtName; //��� ��������������� ���������

    PGconn *SqlConnection; //��������� �� ���������� � �����
    PGresult *SqlResult; //��������� � ����������� �������
    int CountRows; //���������� ����� � ���������� �������
    int CountColumns; //���������� ����� � ���������� �������
    int CurrentRow; //������ ������� ������ �������������� �������
    bool IsSelect; //������ �������� SELECT ��� SHOW

    //std::map<size_t, ISSqlField> ColumnMap; //����-������ �������
};
//-----------------------------------------------------------------------------
#endif
