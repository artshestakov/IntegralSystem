#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGSequence
{
public:
	static bool CreateSequence(const QString &TableName, QString &ErrorString); //�������� ������������������
	static bool CheckExistSequence(const QString &TableName, bool &Exist, QString &ErrorString); //�������� ������������� ������������������
	static QString GetSequenceNameForTable(const QString &TableName); //��������� ����� ������������������
};
//-----------------------------------------------------------------------------
