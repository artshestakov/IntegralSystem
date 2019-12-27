#include "StdAfx.h"
#include "ISButtonWeather.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISSettings.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISButtonWeather::ISButtonWeather(QWidget *parent) : ISPushButton(parent)
{
	Timer = nullptr;

	setFlat(true);
	setEnabled(false);
	setText(LOCALIZATION("Weather").arg(LOCALIZATION("GettingData") + "..."));
	setIcon(BUFFER_ICONS("Weather.Loading"));
	setCursor(CURSOR_WAIT);

	connect(this, &ISButtonWeather::clicked, this, &ISButtonWeather::ReloadWeather);

	OpenWeatherMap = new ISOpenWeatherMap(this);
	connect(OpenWeatherMap, &ISOpenWeatherMap::Answer, this, &ISButtonWeather::WeatherAnswer);
	connect(OpenWeatherMap, &ISOpenWeatherMap::Error, this, &ISButtonWeather::AnswerError);
	OpenWeatherMap->StartQuery();

	int TimeoutInterval = SETTING_INT(CONST_UID_SETTING_VIEW_WEATHERTIMEOUT) * 60000;
	if (TimeoutInterval)
	{
		Timer = new QTimer(this);
		Timer->setInterval(TimeoutInterval);
		connect(Timer, &QTimer::timeout, this, &ISButtonWeather::ReloadWeather);
		Timer->start();
	}
}
//-----------------------------------------------------------------------------
ISButtonWeather::~ISButtonWeather()
{

}
//-----------------------------------------------------------------------------
void ISButtonWeather::ReloadWeather()
{
	setEnabled(false);
	setText(LOCALIZATION("Weather").arg(LOCALIZATION("GettingData") + "..."));
	setIcon(BUFFER_ICONS("Weather.Loading"));
	setCursor(CURSOR_WAIT);

	OpenWeatherMap->StartQuery();
}
//-----------------------------------------------------------------------------
void ISButtonWeather::WeatherAnswer(const QString &IconName, int Temp, const QString &CityName, const QString &Description, int Pressure, int WindDirection, int WindSpeed, double Humidity)
{
	QString TempString;

	if (Temp > 0)
	{
		TempString = QString("+%1").arg(Temp);
	}
	else
	{
		TempString = QString::number(Temp);
	}

	setIcon(BUFFER_ICONS("Weather." + IconName));
	setText(LOCALIZATION("Weather").arg(QString::fromLocal8Bit("%1°C").arg(TempString)));
	setCursor(CURSOR_POINTING_HAND);
	setEnabled(true);

	QString ToolTip;
	ToolTip += QString(LOCALIZATION("Weather.CityName") + ": %1\n").arg(CityName);
	ToolTip += QString("%1\n").arg(Description);
	ToolTip += QString(LOCALIZATION("Weather.Pressure") + ": %3" + LOCALIZATION("Weather.Pressure.Reduction") + "\n").arg(Pressure);
	ToolTip += QString(LOCALIZATION("Weather.Wind") + ": %1, %2 " + LOCALIZATION("Weather.Wind.Reduction") + "\n").arg(GetWindDirection(WindDirection)).arg(WindSpeed);
	ToolTip += QString(LOCALIZATION("Weather.Humidity") + ": %1").arg(Humidity);
	setToolTip(ToolTip);
}
//-----------------------------------------------------------------------------
void ISButtonWeather::AnswerError(const QString &ErrorString)
{
	Timer->stop();

	setText(LOCALIZATION("Weather").arg(LOCALIZATION("Weather.Error") + "..."));
	setToolTip(ErrorString);
	setCursor(CURSOR_POINTING_HAND);
	setEnabled(true);

	ISMessageBox MessageBox(ISMessageBox::Warning, LOCALIZATION("Warning"), LOCALIZATION("Message.Warning.WeatherAnswer"), ISMessageBox::Ok, this);
	MessageBox.setDetailedText(ErrorString);
	MessageBox.setCheckBox(new QCheckBox(LOCALIZATION("Message.Warning.WeatherAnswer.CheckBoxText"), &MessageBox));
	MessageBox.Exec();

	if (MessageBox.checkBox()->isChecked())
	{
		Timer->stop();
		ISSettings::GetInstance().SaveValue(CONST_UID_SETTING_VIEW_SHOWWEATHERWIDGET, false);
		close();
		return;
	}

	Timer->start();
}
//-----------------------------------------------------------------------------
QString ISButtonWeather::GetWindDirection(int WindDirection) const
{
	QString Result;

	if (WindDirection == 0) //Северный
	{
		Result = LOCALIZATION("Weather.Wind.Direction.Northern");
	}
	else if (WindDirection > 0 && WindDirection < 90) //Северо-восточный
	{
		Result = LOCALIZATION("Weather.Wind.Direction.NorthEast");
	}
	else if (WindDirection == 90) //Восточный
	{
		Result = LOCALIZATION("Weather.Wind.Direction.Oriental");
	}
	else if (WindDirection > 90 && WindDirection < 180) //Юго-восточный
	{
		Result = LOCALIZATION("Weather.Wind.Direction.SouthEast");
	}
	else if (WindDirection == 180) //Южный
	{
		Result = LOCALIZATION("Weather.Wind.Direction.Southerly");
	}
	else if (WindDirection > 180 && WindDirection < 270) //Юго-западный
	{
		Result = LOCALIZATION("Weather.Wind.Direction.SouthWest");
	}
	else if (WindDirection == 270) //Западный
	{
		Result = LOCALIZATION("Weather.Wind.Direction.West");
	}
	else if (WindDirection > 270 && WindDirection < 360) //Северо-западный
	{
		Result = LOCALIZATION("Weather.Wind.Direction.NorthWest");
	}

	return Result;
}
//-----------------------------------------------------------------------------
