#pragma once
#ifndef _ISVALIDATORS_H_INCLUDED
#define _ISVALIDATORS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
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
    ISDoubleValidator(int Decimals, QObject *parent = 0);
    virtual ~ISDoubleValidator();

    QDoubleValidator::State validate(QString &String, int &Pos) const;
};
//-----------------------------------------------------------------------------
class ISMoneyValidator : public ISDoubleValidator
{
    Q_OBJECT

public:
    ISMoneyValidator(QObject *parent = 0);
    virtual ~ISMoneyValidator();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
