#pragma once
#ifndef _ISTCPQUERY_H_INCLUDED
#define _ISTCPQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISTcpQuery
{
public:
    ISTcpQuery(const QString &query_type);
    ~ISTcpQuery();

    QString GetErrorString() const; //�������� ��������� �������� ������
    unsigned int GetVersion() const; //�������� ������ ���������
    void BindValue(const QString &ParamterName, const QVariant &ParameterValue); //������� ���������
    virtual bool Execute(const QString &query_type); //��������� ������ �� �����
    virtual bool Execute(); //��������� ������
    QVariantMap& GetAnswer(); //�������� ������ �� �����
    QVariantMap TakeAnswer(); //�������� ����� �� ������

protected:
    QVariant GetParameter(const QString &ParameterName) const; //�������� �������� �� �����

private:
    bool IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap); //�������� ���������� ������

private:
    QString ErrorString; //�������� �������� ������
    QString QueryType; //��� �������
    QVariantMap Parameters; //��������� �������
    QVariantMap TcpAnswer; //�����
    unsigned int Version; //������ ���������
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTcpQueryTable : public ISTcpQuery
{
public:
    ISTcpQueryTable();
    virtual ~ISTcpQueryTable();

    bool Execute() override;
    void SetSorting(const QString &SortingField, Qt::SortOrder SortingOrder); //�������� ����������
    void SetSearch(const QVariantList &VariantMap); //�������� ��������� �������
    void AddFilter(const QString &FieldName, const QVariant &Value); //�������� ������

private:
    QVariantMap FilterMap;
    QVariantMap SortingMap;
};
//-----------------------------------------------------------------------------
#endif
