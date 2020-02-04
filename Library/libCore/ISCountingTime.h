#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCountingTime
{
public:
	ISCountingTime();
	~ISCountingTime();

	int GetElapsed(); //�������� ��������� ������ � ������������
	int GetElapsedRestart(); //�������� ��������� ������ � ������������ � �������� ������
	void Restart(); //��������� �������� �������

private:
	QTime Time;
	int Elapsed;
};
//-----------------------------------------------------------------------------
