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
    ISTcpQuery(const std::string &query_type);
    ~ISTcpQuery();

    const std::string& GetErrorString() const; //�������� ��������� �������� ������
    void BindString(const std::string &ParamterName, const std::string &String); //������� ���������
    virtual bool Execute(const std::string &query_type); //��������� ������ �� �����
    virtual bool Execute(); //��������� ������
    //QVariantMap& GetAnswer(); //�������� ������ �� �����
    //QVariantMap TakeAnswer(); //�������� ����� �� ������

protected:
    //QVariant GetParameter(const QString &ParameterName) const; //�������� �������� �� �����

private:
    bool IsValidAnswer(const std::string &StringBuffer, size_t Size, rapidjson::Value &ValueIsError, rapidjson::Value &ValueErrorString); //�������� ���������� ������
    bool CheckConnection(); //�������� ����������

private:
    std::string ErrorString; //�������� �������� ������
    std::string QueryType; //��� �������
    rapidjson::Document Parameters; //��������� �������
    rapidjson::Value AnswerObject; //�����

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
