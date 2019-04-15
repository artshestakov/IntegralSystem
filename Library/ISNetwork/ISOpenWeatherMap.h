#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISOpenWeatherMap : public QObject
{
	Q_OBJECT

signals:
	void Answer(const QString &IconName, int Temp, const QString &CityName, const QString &Description, int Pressure, int WindDirection, int WindSpeed, double Humidity);
	void Error(const QString &ErrorString);

public:
	ISOpenWeatherMap(QObject *parent = 0);
	virtual ~ISOpenWeatherMap();

	void StartQuery(); //Запуск запроса к погоде

protected:
	void QueryWeather(); //Запрос к погоде

private:
	QFutureWatcher<void> *FutureWatcher;
};
//-----------------------------------------------------------------------------
