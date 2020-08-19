#include "ISNetwork.h"
#include "ISConstants.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISNetwork::ISNetwork(QObject *parent)
	: QObject(parent),
	ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISNetwork::~ISNetwork()
{

}
//-----------------------------------------------------------------------------
QString ISNetwork::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISNetwork::CheckAccessInternet(QString &error_string) const
{
	QEventLoop EventLoop;
	QNetworkAccessManager NetworkAccessManager;
	connect(&NetworkAccessManager, &QNetworkAccessManager::finished, &EventLoop, &QEventLoop::quit);

	QNetworkRequest NetworkRequest(QUrl("http://google.com"));
	QNetworkReply *NetworkReply = NetworkAccessManager.get(NetworkRequest);
	EventLoop.exec();

	bool Result = NetworkReply ? true : false;
	if (Result)
	{
		if (NetworkReply->error() != QNetworkReply::NoError)
		{
			error_string = NetworkReply->errorString();
			ISLOGGER_W(NetworkReply->errorString());
		}
		delete NetworkReply;
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
bool ISNetwork::DownloadFile(const QString &Url, QByteArray &ByteArray)
{
	QEventLoop EventLoop;
	QNetworkAccessManager NetworkAccessManager;
	connect(&NetworkAccessManager, &QNetworkAccessManager::finished, &EventLoop, &QEventLoop::quit);

	QNetworkRequest NetworkRequest(Url);
	QNetworkReply *NetworkReply = NetworkAccessManager.get(NetworkRequest);
	EventLoop.exec();

	bool Result = NetworkReply ? true : false;
	if (Result)
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
	return Result;
}
//-----------------------------------------------------------------------------
