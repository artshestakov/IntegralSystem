#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCountingTime
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
