#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCountingTime
{
public:
	ISCountingTime(QObject *parent = 0);
	virtual ~ISCountingTime();

	int GetElapsed(); //�������� ��������� ������ � ������������
	int GetElapsedRestart(); //�������� ��������� ������ � ������������ � �������� ������
	void Restart(); //��������� �������� �������

private:
	QTime Time;
	int Elapsed;
};
//-----------------------------------------------------------------------------
