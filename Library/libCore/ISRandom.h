#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISRandom
{
public:
	ISRandom(QObject *parent = 0);
	virtual ~ISRandom();

	static int Integer(); //����� ��������� �����
	static int Integer(int Minimum, int Maximum); //����� ��������� �����
	static double Double(); //����� � ��������� �������
	static QString String(int Symbols = 5); //������
};
//-----------------------------------------------------------------------------
