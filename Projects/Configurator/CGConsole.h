#pragma once
#ifndef _CGCONSOLE_H_INCLUDED
#define _CGCONSOLE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGConsole
{
public:
	static bool Question(const QString &QuestionText); //������ ������ ������������
	static void Pause(); //����� �����
	static QString GetString(const QString &OutputString = QString()); //������ �� ���� ������
	static int GetInt(const QString &OutputString = QString()); //������ �� ���� ������ �����
};
//-----------------------------------------------------------------------------
#endif
