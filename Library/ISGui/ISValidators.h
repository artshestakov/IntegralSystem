#pragma once
#ifndef _ISVALIDATORS_H_INCLUDED
#define _ISVALIDATORS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISDoubleValidator : public QDoubleValidator
{
	Q_OBJECT

public:
	ISDoubleValidator(int Decimals, QObject *parent = 0);
	virtual ~ISDoubleValidator();

	QValidator::State validate(QString &String, int &Position) const override;
};
//-----------------------------------------------------------------------------
#endif
