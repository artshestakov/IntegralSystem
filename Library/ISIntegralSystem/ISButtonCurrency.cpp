#include "StdAfx.h"
#include "ISButtonCurrency.h"
#include "EXDefines.h"
#include "EXConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISSettings.h"
//-----------------------------------------------------------------------------
ISButtonCurrency::ISButtonCurrency(QWidget *parent) : ISPushButton(parent)
{
	Timer = nullptr;

	setFlat(true);
	setEnabled(false);
	setText(LOCALIZATION("Currency.GettingData") + "...");
	setToolTip(LOCALIZATION("Currency.ToolTip"));
	setIcon(BUFFER_ICONS("Currency.Loading"));
	setCursor(CURSOR_WAIT);

	connect(this, &ISButtonCurrency::clicked, this, &ISButtonCurrency::ReloadCurrency);

	CBR = new ISCBR(this);
	connect(CBR, &ISCBR::Answer, this, &ISButtonCurrency::CurrencyAnswer);
	connect(CBR, &ISCBR::Error, this, &ISButtonCurrency::AnswerError);
	CBR->StartQuery();

	int TimeoutInterval = SETTING_INT(CONST_UID_SETTING_VIEW_CURRENCYTIMEOUT) * 60000;
	if (TimeoutInterval)
	{
		Timer = new QTimer(this);
		Timer->setInterval(TimeoutInterval);
		connect(Timer, &QTimer::timeout, this, &ISButtonCurrency::ReloadCurrency);
		Timer->start();
	}
}
//-----------------------------------------------------------------------------
ISButtonCurrency::~ISButtonCurrency()
{

}
//-----------------------------------------------------------------------------
void ISButtonCurrency::ReloadCurrency()
{
	setEnabled(false);
	setText(LOCALIZATION("Currency.GettingData") + "...");
	setIcon(BUFFER_ICONS("Currency.Loading"));
	setCursor(CURSOR_WAIT);

	CBR->StartQuery();
}
//-----------------------------------------------------------------------------
void ISButtonCurrency::CurrencyAnswer(const QString &Dollar, const QString &Euro)
{
	setText(LOCALIZATION("Currency.Text").arg(Dollar).arg(Euro));
	setToolTip(LOCALIZATION("Currency.ToolTip"));
	setIcon(BUFFER_ICONS("Currency"));
	setCursor(CURSOR_POINTING_HAND);
	setEnabled(true);
}
//-----------------------------------------------------------------------------
void ISButtonCurrency::AnswerError(const QString &ErrorString)
{
	Timer->stop();

	setText(LOCALIZATION("Currency.Error"));
	setToolTip(ErrorString);
	setCursor(CURSOR_POINTING_HAND);
	setEnabled(true);

	ISMessageBox MessageBox(ISMessageBox::Warning, LOCALIZATION("Warning"), LOCALIZATION("Message.Warning.CurrencyAnswer"), ISMessageBox::Ok, this);
	MessageBox.setDetailedText(ErrorString);
	MessageBox.setCheckBox(new QCheckBox(LOCALIZATION("Message.Warning.CurrencyAnswer.CheckBoxText"), &MessageBox));
	MessageBox.Exec();

	if (MessageBox.checkBox()->isChecked())
	{
		Timer->stop();
		ISSettings::GetInstance().SaveValue(CONST_UID_SETTING_VIEW_SHOWCURRENCYWIDGET, false);
		close();
		return;
	}

	Timer->start();
}
//-----------------------------------------------------------------------------
