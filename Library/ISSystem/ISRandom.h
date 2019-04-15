#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISRandom : public QObject
{
	Q_OBJECT

public:
	ISRandom(QObject *parent = 0);
	virtual ~ISRandom();

	static int Integer(); //����� ��������� �����
	static int Integer(int Minimum, int Maximum); //����� ��������� �����
	static double Double(); //����� � ��������� �������
	static QString String(int Symbols = 5); //������
};
//-----------------------------------------------------------------------------
