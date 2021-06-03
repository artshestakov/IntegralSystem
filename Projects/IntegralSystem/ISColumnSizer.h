#pragma once
#ifndef _ISCOLUMNSIZER_H_INCLUDED
#define _ISCOLUMNSIZER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISColumnSizer
{
public:
    static ISColumnSizer& Instance();

    void Initialize(const QVariantMap &VariantMap);
    QVariantMap GetColumnSize() const; //�������� ������ � �������� �����

    void SetColumnSize(const QString &TableName, const QString &FieldName, int Size); //�������� ������ ���� � �������
    int GetColumnSize(const QString &TableName, const QString &FieldName) const; //�������� ������ ���� � �������

private:
    ISColumnSizer();
    ~ISColumnSizer();
    ISColumnSizer(const ISColumnSizer&) = delete;
    ISColumnSizer(ISColumnSizer&&) = delete;
    ISColumnSizer& operator=(const ISColumnSizer&) = delete;
    ISColumnSizer& operator=(ISColumnSizer&&) = delete;

private:
    std::map<QString, ISStringToIntMap> Tables;
};
//-----------------------------------------------------------------------------
#endif
