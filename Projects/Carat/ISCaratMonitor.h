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
	void IncomingQuerySize(int QuerySize); //����������� ������� ��������� �������
	void AnswerQuerySize(int AnswerSize); //����������� ������� ������ �� ������

private:
	ISCaratMonitor();
	~ISCaratMonitor();
	ISCaratMonitor(const ISCaratMonitor&) = delete;
	ISCaratMonitor(ISCaratMonitor&&) = delete;
	ISCaratMonitor& operator=(const ISCaratMonitor&) = delete;
	ISCaratMonitor& operator=(ISCaratMonitor&&) = delete;

private:
	int _IncomingQuerySize; //������ ��������� ��������
	int _AnswerQuerySize; //������ ���������� ��������
};
//-----------------------------------------------------------------------------
#endif
