#include "StdAfx.h"
#include "ISOpenWeatherMap.h"
#include "EXDefines.h"
#include "ISTrace.h"
//-----------------------------------------------------------------------------
ISOpenWeatherMap::ISOpenWeatherMap(QObject *parent) : QObject(parent)
{
	FutureWatcher = new QFutureWatcher<void>(this);
}
//-----------------------------------------------------------------------------
ISOpenWeatherMap::~ISOpenWeatherMap()
{

}
//-----------------------------------------------------------------------------
void ISOpenWeatherMap::StartQuery()
{
	QFuture<void> Future = QtConcurrent::run(this, &ISOpenWeatherMap::QueryWeather);
	FutureWatcher->setFuture(Future);
}
//-----------------------------------------------------------------------------
void ISOpenWeatherMap::QueryWeather()
{
	QNetworkAccessManager Manager;
	QNetworkRequest Request(QUrl(QString("http://api.openweathermap.org/data/2.5/find?q=Stavropol,RU&type=like&appid=%1&lang=ru").arg(TOKEN_OPEN_WEATHER_MAP_TOKEN)));
	QNetworkReply *Reply = Manager.get(Request);

	QEventLoop EventLoop;
	connect(Reply, &QNetworkReply::finished, &EventLoop, &QEventLoop::quit);
	EventLoop.exec();

	if (Reply->error() == QNetworkReply::NoError)
	{
		QString ReplyString = Reply->readAll();
		QJsonObject JsonObject = QJsonDocument::fromJson(ReplyString.toUtf8()).object();
		QVariantMap VariantMap = JsonObject.toVariantMap();
		QVariant VariantList = VariantMap.value("list").toList().at(0);
		VariantMap = VariantList.toMap();

		int Temp = round(VariantMap.value("main").toMap().value("temp").toDouble() - 273); //Перевод в градусы Цельсия и округление до близжайшего целого
		QString IconName = VariantMap.value("weather").toList().at(0).toMap().value("icon").toString(); //Наименование картинки

		QString CityName = VariantMap.value("name").toString(); //Город
		QString Descroption = VariantMap.value("weather").toList().at(0).toMap().value("description").toString(); //Описание
		int Pressure = VariantMap.value("main").toMap().value("pressure").toInt() * 0.75006375541921; //Давление
		int WindDirection = VariantMap.value("wind").toMap().value("deg").toInt(); //Градус направления ветра
		int WindSpeed = VariantMap.value("wind").toMap().value("speed").toInt(); //Градус направления ветра
		double Humidity = VariantMap.value("main").toMap().value("humidity").toDouble(); //Влажность

		emit Answer(IconName, Temp, CityName, Descroption, Pressure, WindDirection, WindSpeed, Humidity);
	}
	else
	{
		emit Error(Reply->errorString());
	}

	if (Reply)
	{
		delete Reply;
		Reply = nullptr;
	}
}
//-----------------------------------------------------------------------------
