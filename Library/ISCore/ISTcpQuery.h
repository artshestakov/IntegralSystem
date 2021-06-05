#pragma once
#ifndef _ISTCPQUERY_H_INCLUDED
#define _ISTCPQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include <rapidjson/document.h>
//-----------------------------------------------------------------------------
class ISTcpQuery
{
public:
    ISTcpQuery(const char *query_type);
    ~ISTcpQuery();

    const std::string& GetErrorString() const; //�������� ��������� �������� ������
    void BindString(const std::string &ParamterName, const std::string &String); //������� ���������
    virtual bool Execute(const char *query_type); //��������� ������ �� �����
    virtual bool Execute(); //��������� ������
    const rapidjson::Value& GetAnswer() const; //�������� ������ �� �����

protected:
    //QVariant GetParameter(const QString &ParameterName) const; //�������� �������� �� �����

private:
    bool IsValidAnswer(const std::string &StringBuffer, size_t Size, bool &ValueIsError, std::string &ValueErrorString); //�������� ���������� ������
    bool CheckConnection(); //�������� ����������

private:
    std::string ErrorString; //�������� �������� ������
    const char *QueryType; //��� �������
    rapidjson::Document Parameters; //��������� �������
    size_t ParametersCount; //���������� ����������
    rapidjson::Document AnswerObject; //�����

};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*class ISTcpQueryTable : public ISTcpQuery
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
};*/
//-----------------------------------------------------------------------------
#endif
