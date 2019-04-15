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

	static int Integer(); //Целое рандомное число
	static int Integer(int Minimum, int Maximum); //Целое рандомное число
	static double Double(); //Число с плавающей запятой
	static QString String(int Symbols = 5); //Строка
};
//-----------------------------------------------------------------------------
