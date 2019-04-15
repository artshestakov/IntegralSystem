#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISCountingTime : public QObject
{
	Q_OBJECT

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
