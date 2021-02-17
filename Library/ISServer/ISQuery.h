#pragma once
#ifndef _ISQUERY_H_INCLUDED
#define _ISQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISQuery
{
public:
    ISQuery(const QString &sql_text = QString(), bool prepare = true);
    ISQuery(const QSqlDatabase &sql_database, const QString &sql_text = QString(), bool prepare = true);
    ~ISQuery();

    bool Prepare(const QString &sql_text);
    bool Prepare(QSqlDatabase &sql_database, const QString &sql_text);

    bool Execute();
    bool Execute(const QString &sql_text);
    bool Execute(QSqlDatabase &sql_database, const QString &sql_text);

    bool ExecuteFirst();
    bool ExecuteFirst(QSqlDatabase &sql_database, const QString &sql_text);

    bool BindValue(const QString &ParameterName, const QVariant &ParameterValue);

    QVariant ReadColumn(const QString &ColumnName);
    QVariant ReadColumn(int ColumnIndex);

    bool Next();
    bool First();

    QSqlRecord GetRecord();

    bool IsValid() const;
    bool IsSelect() const;
    bool ExistParameter(const QString &ParameterName) const;
    QString GetSqlText() const; //�������� ����� �������
    QString GetErrorString(); //�������� ����� ������
	QSqlError::ErrorType GetErrorType(); //�������� ��� ������
    int GetErrorNumber() const; //�������� ����� ������
    int GetCountResultRows() const; //�������� ���������� ����� �� ���������� ������� (������ ��� SELECT)
    int GetCountAffected() const; //�������� ���������� ���������� �������� �����
    void SetShowLongQuery(bool show_long_query);

private:
    void PrepareColumnIndices();

private:
    QString ErrorString;
    QSqlQuery SqlQuery;
    QString SqlText;
    ISStringToIntMap ColumnIndices;
    QSqlDatabase SqlDatabase;
    bool ShowLongQuery;
};
//-----------------------------------------------------------------------------
#endif
