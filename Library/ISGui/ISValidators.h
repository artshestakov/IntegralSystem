#pragma once
#ifndef _ISVALIDATORS_H_INCLUDED
#define _ISVALIDATORS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISIntValidator : public QIntValidator
{
	Q_OBJECT

public:
	ISIntValidator(int Bottom, int Top, QObject *parent = 0);
	ISIntValidator(QObject *parent = 0);
	virtual ~ISIntValidator();

	QIntValidator::State validate(QString &String, int &Pos) const;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDoubleValidator : public QDoubleValidator
{
	Q_OBJECT

public:
	ISDoubleValidator(double Bottom, double Top, int Decimals, QObject *parent = 0);
	ISDoubleValidator(QObject *parent = 0);
	virtual ~ISDoubleValidator();

	QDoubleValidator::State validate(QString &String, int &Pos) const;

private:
	int Decimal; //Количество чисел после разделителя
};
//-----------------------------------------------------------------------------
#endif
