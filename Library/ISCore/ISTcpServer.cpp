#include "ISTcpServer.h"
#include "ISTcp.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISTcpServer::ISTcpServer(QObject *parent)
	: QTcpServer(parent),
	ErrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
ISTcpServer::~ISTcpServer()
{

}
//-----------------------------------------------------------------------------
QString ISTcpServer::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISTcpServer::Run(quint16 Port)
{
	bool Result = listen(QHostAddress::Any, Port);
	if (Result)
	{
		connect(this, &ISTcpServer::acceptError, this, &ISTcpServer::AcceptError);
	}
	else
	{
		ErrorString = errorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpServer::incomingConnection(qintptr SocketDescriptor)
{
	QTcpServer::incomingConnection(SocketDescriptor);
	QTcpSocket *TcpSocket = nextPendingConnection();
	connect(TcpSocket, &QTcpSocket::disconnected, TcpSocket, &QTcpSocket::deleteLater);

	//∆дЄм запроса на авторизацию
	QByteArray ByteArray;
	while (true)
	{
		TcpSocket->waitForReadyRead();
		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (ByteArray.right(CARAT_PACKET_SEPARATOR_SIZE) == CARAT_PACKET_SEPARATOR)
			{
				ByteArray.remove(ByteArray.size() - CARAT_PACKET_SEPARATOR_SIZE, CARAT_PACKET_SEPARATOR_SIZE);
				break;
			}
		}
	}
	
	QVariantMap VariantMap;
	QString ErrorString;
	if (!ISTcp::IsValidQuery(ByteArray, VariantMap, ErrorString))
	{
		SendError(TcpSocket, ErrorString);
		return;
	}

	if (VariantMap["Type"].toString() != "Auth")
	{
		SendError(TcpSocket, "Invalid query type");
		return;
	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::AcceptError(QAbstractSocket::SocketError socket_error)
{
	ErrorString = errorString();
}
//-----------------------------------------------------------------------------
void ISTcpServer::SendError(QTcpSocket *TcpSocket, QString ErrorString)
{
	//ќтправл€ем ошибку
	qint64 WriteSize = TcpSocket->write(ISSystem::VariantMapToJsonString(
	{
		{ "IsError", true },
		{ "Description", ErrorString }
	}).simplified().toUtf8() + CARAT_PACKET_SEPARATOR);
	TcpSocket->flush();

	//∆дЄм пока данные уйдут
	TcpSocket->waitForBytesWritten();
	TcpSocket->abort();
}
//-----------------------------------------------------------------------------
