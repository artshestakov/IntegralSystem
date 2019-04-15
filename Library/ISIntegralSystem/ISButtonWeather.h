#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISPushButton.h"
#include "ISOpenWeatherMap.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISButtonWeather : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonWeather(QWidget *parent = 0);
	virtual ~ISButtonWeather();

protected:
	void ReloadWeather();
	void WeatherAnswer(const QString &IconName, int Temp, const QString &CityName, const QString &Description, int Pressure, int WindDirection, int WindSpeed, double Humidity);
	void AnswerError(const QString &ErrorString);
	QString GetWindDirection(int WindDirection) const;

private:
	ISOpenWeatherMap *OpenWeatherMap;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
