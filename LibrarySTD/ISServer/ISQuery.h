#pragma once
#ifndef _ISQUERY_H_INCLUDED
#define _ISQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
#include "ISDateTime.h"
#include "ISQueryText.h"
#include "libpq-fe.h"
#include "pg_type_d.h"
//-----------------------------------------------------------------------------
class ISQuery
{
public:
    ISQuery(PGconn *sql_connection, const std::string &sql_text = std::string());
    ISQuery(const std::string &sql_text);
    ~ISQuery();

    const std::string& GetErrorString() const; //�������� ��������� �������� ������
    int GetErrorNumber() const; //�������� ��� ������
    
    const std::string& GetSqlText() const; //�������� ����� �������
    void SetSqlQuery(const std::string &sql_text); //�������� ����� �������
    
    int GetResultRowCount() const; //�������� ������ �������
    int GetResultColumnCount() const; //�������� ���-�� ����� � �������
    Oid GetResultColumnType(size_t Index) const; //�������� ��� ���� � �������������� �������
    const char* GetResultFieldName(int Index) const; //�������� ��� ���� �� �������
    size_t GetResultAffected() const; //�������� ���������� ��������� �����
    bool GetIsSelect() const; //���������, �������� �� ������ ��������

    void SetShowLongQuery(bool show_long_query);

    bool First();
    bool Next();

    void Bind(const std::string &String);
    void BindNull();
    void BindString(const std::string &String);
    void BindString(const char *String);
    void BindUID(const std::string &UID);
    void BindUID(const char *UID);
    void BindBool(bool Value);
    void BindBinary(unsigned char *Data, size_t Size);
    void BindUInt(unsigned int Value);
    void BindUInt64(uint64_t Value);
    void BindInt(int Value);
    void BindInt64(int64_t Value);
    void BindDouble(double Value);
    void BindDate(const ISDate &Date);

    bool Execute();
    bool ExecuteFirst();

    bool IsNull(size_t Index) const;
    char* ReadColumn(size_t Index) const;
    std::string ReadColumn_String(size_t Index) const;
    int ReadColumn_Int(size_t Index) const;
    double ReadColumn_Double(size_t Index) const;
    unsigned int ReadColumn_UInt(size_t Index) const;
    uint64_t ReadColumn_Int64(size_t Index) const;
    uint64_t ReadColumn_UInt64(size_t Index) const;
    bool ReadColumn_Bool(size_t Index) const;
    ISDate ReadColumn_Date(size_t Index) const;
    ISTime ReadColumn_Time(size_t Index) const;
    ISDateTime ReadColumn_DateTime(size_t Index) const;
    unsigned char* ReadColumn_Binary(size_t Index) const;
    unsigned char* ReadColumn_Binary(size_t Index, size_t &DataSize) const;

private:
    bool Prepare(int ParamCount);
    void ClearResult();

private:
    std::string ErrorString; //�������� �������� ������
    int ErrorNumber; //��� ������
    char *ErrorSeverity; //����������� ������
    char *ErrorPrimary; //������� �������� ������
    char *ErrorDetail; //������ ������
    char *ErrorHint; //��������� �� ���������� ������

    bool ShowLongQuery; //���������� ������ ������
    std::string SqlText; //����� �������

    std::vector<char *> ParameterValues; //��������� �������
    std::vector<Oid> ParameterTypes; //���� ���������� �������
    std::vector<int> ParameterFormats; //������� ����������
    std::vector<int> ParameterLengths;
    size_t ParameterCount; //���������� ����������

    std::string StmtName; //��� ��������������� ���������

    PGconn *SqlConnection; //��������� �� ���������� � �����
    PGresult *SqlResult; //��������� � ����������� �������
    int CountRows; //���������� ����� � ���������� �������
    int CountColumns; //���������� ����� � ���������� �������
    int CurrentRow; //������ ������� ������ �������������� �������
    bool IsSelect; //������ �������� SELECT ��� SHOW
};
//-----------------------------------------------------------------------------
#endif
