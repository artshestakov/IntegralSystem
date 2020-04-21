#include "ISNetwork.h"
#include "ISConstants.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISNetwork::ISNetwork(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------------------------
ISNetwork::~ISNetwork()
{

}
//-----------------------------------------------------------------------------
bool ISNetwork::CheckAccessInternet(QString &ErrorString) const
{
	bool Result = true;
	QEventLoop EventLoop;
	QNetworkAccessManager NetworkAccessManager;
	connect(&NetworkAccessManager, &QNetworkAccessManager::finished, &EventLoop, &QEventLoop::quit);

	QNetworkRequest NetworkRequest(QUrl("http://google.com"));
	QNetworkReply *NetworkReply = NetworkAccessManager.get(NetworkRequest);
	EventLoop.exec();

	if (NetworkReply)
	{
		if (NetworkReply->error() != QNetworkReply::NoError)
		{
			ErrorString = NetworkReply->errorString();
			ISLOGGER_W(NetworkReply->errorString());
		}

		delete NetworkReply;
	}
	else
	{
		Result = false;
	}

	return Result;
}
//-----------------------------------------------------------------------------
QString ISNetwork::ParseIPAddress(const QString &IPAddress) const
{
	int Pos = 0;
	QString Result;
	QRegExp RegExp(REG_EXP_IP_ADDRESS);
	while ((Pos = RegExp.indexIn(IPAddress, Pos)) != -1)
	{
		Result = RegExp.cap(0);
		Pos += RegExp.matchedLength();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISNetwork::DownloadFile(const QString &Url, QByteArray &ByteArray, QString &ErrorString) const
{
	bool Result = true;
	QEventLoop EventLoop;
	QNetworkAccessManager NetworkAccessManager;
	connect(&NetworkAccessManager, &QNetworkAccessManager::finished, &EventLoop, &QEventLoop::quit);

	QNetworkRequest NetworkRequest(Url);
	QNetworkReply *NetworkReply = NetworkAccessManager.get(NetworkRequest);
	EventLoop.exec();

	if (NetworkReply)
	{
		if (NetworkReply->error() == QNetworkReply::NoError)
		{
			ByteArray = NetworkReply->readAll();
		}
		else
		{
			ErrorString = NetworkReply->errorString();
		}
		delete NetworkReply;
	}
	else
	{
		Result = false;
	}

	return Result;
}
//-----------------------------------------------------------------------------
