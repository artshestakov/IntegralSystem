#pragma once
#ifndef _ISCONSOLE_H_INCLUDED
#define _ISCONSOLE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISConsole
{
public:
	static bool Question(const QString &QuestionText); //������ ������ ������������
	static void Pause(); //����� �����
	static QString GetString(const QString &OutputString = QString()); //������ �� ���� ������
	static int GetInt(const QString &OutputString = QString()); //������ �� ���� ������ �����
	static bool InstallEncoding(unsigned int CodePage); //���������� ������� ��������
};
//-----------------------------------------------------------------------------
#endif
