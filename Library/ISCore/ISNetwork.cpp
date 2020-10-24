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
