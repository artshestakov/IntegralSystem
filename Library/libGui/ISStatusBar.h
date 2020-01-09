#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISPushButton.h"
#include "ISButtonWeather.h"
#include "ISButtonCurrency.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISStatusBar : public QStatusBar
{
	Q_OBJECT

signals:
	void DateTimeClicked();
	void MakeCall();
	void TrayClicked();

public:
	ISStatusBar(QWidget *parent = 0);
	virtual ~ISStatusBar();

protected:
	void CreateDateTime(); //Виджет с текущей датой и временем
	void CreateConnectData(); //Виджет с данными подключения к БД

	void TimerTick();
	QString GetCurrentDateTime() const;

private:
	QFont Font;	
	ISPushButton *ButtonDateTime;

	ISButtonWeather *ButtonWeather;
	ISButtonCurrency *ButtonCurrency;
};
//-----------------------------------------------------------------------------
