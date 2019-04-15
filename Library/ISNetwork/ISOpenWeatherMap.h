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

	void StartQuery(); //������ ������� � ������

protected:
	void QueryWeather(); //������ � ������

private:
	QFutureWatcher<void> *FutureWatcher;
};
//-----------------------------------------------------------------------------
