#pragma once
#ifndef _ISCARATMONITOR_H_INCLUDED
#define _ISCARATMONITOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCaratMonitor
{
public:
	static ISCaratMonitor& Instance();

public:
	void IncomingQuerySize(quint64 QuerySize); //����������� ������� ��������� �������
	void AnswerQuerySize(quint64 AnswerSize); //����������� ������� ������ �� ������

private:
	ISCaratMonitor();
	~ISCaratMonitor();
	ISCaratMonitor(ISCaratMonitor const &) {};
	ISCaratMonitor& operator=(ISCaratMonitor const&) { return *this; };

private:
	quint64 _IncomingQuerySize; //������ ��������� ��������
	quint64 _AnswerQuerySize; //������ ���������� ��������
};
//-----------------------------------------------------------------------------
#endif
