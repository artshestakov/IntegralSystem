#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISPushButton.h"
#include "ISCBR.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISButtonCurrency : public ISPushButton
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
