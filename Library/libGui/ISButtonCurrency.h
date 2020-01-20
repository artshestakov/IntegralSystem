#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISPushButton.h"
#include "ISCBR.h"
//-----------------------------------------------------------------------------
class ISButtonCurrency : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonCurrency(QWidget *parent = 0);
	virtual ~ISButtonCurrency();

protected:
	void ReloadCurrency();
	void CurrencyAnswer(const QString &Dollar, const QString &Euro);
	void AnswerError(const QString &ErrorString);

private:
	ISCBR *CBR;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
