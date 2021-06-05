#pragma once
#ifndef _ISCONSOLE_H_INCLUDED
#define _ISCONSOLE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISConsole
{
public:
	static bool Question(const std::string &QuestionText); //������ ������ ������������
	static void Pause(); //����� �����
	static std::string GetString(const std::string &OutputString = std::string()); //������ �� ���� ������
	static int GetInt(const std::string &OutputString = std::string()); //������ �� ���� ������ �����
	static bool InstallEncoding(unsigned int CodePage, std::string &ErrorString); //���������� ������� ��������
};
//-----------------------------------------------------------------------------
#endif
