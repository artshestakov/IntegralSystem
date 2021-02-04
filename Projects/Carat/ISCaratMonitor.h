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
	ISCaratMonitor(const ISCaratMonitor&) = delete;
	ISCaratMonitor(ISCaratMonitor&&) = delete;
	ISCaratMonitor& operator=(const ISCaratMonitor&) = delete;
	ISCaratMonitor& operator=(ISCaratMonitor&&) = delete;

private:
	quint64 _IncomingQuerySize; //������ ��������� ��������
	quint64 _AnswerQuerySize; //������ ���������� ��������
};
//-----------------------------------------------------------------------------
#endif
