#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGSequence
{
public:
	static bool CreateSequence(const QString &ClassName); //�������� ������������������
	static bool CheckExistSequence(const QString &ClassName, bool &Exist, QString &ErrorString); //�������� ������������� ������������������
	static QString GetSequenceNameForClass(const QString &ClassName); //��������� ����� ������������������
};
//-----------------------------------------------------------------------------
