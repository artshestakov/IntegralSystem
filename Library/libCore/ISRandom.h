#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISRandom
{
public:
	ISRandom(QObject *parent = 0);
	virtual ~ISRandom();

	static int Integer(); //Целое рандомное число
	static int Integer(int Minimum, int Maximum); //Целое рандомное число
	static double Double(); //Число с плавающей запятой
	static QString String(int Symbols = 5); //Строка
};
//-----------------------------------------------------------------------------
