#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISRandom
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
