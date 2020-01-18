#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCommandLine
{
public:
	ISCommandLine();
	virtual ~ISCommandLine();

	static bool Question(const QString &QuestionText); //������ ������ ������������
	static void Pause(); //����� �����
	static QString GetText(); //������ �� ���� ������
};
//-----------------------------------------------------------------------------
