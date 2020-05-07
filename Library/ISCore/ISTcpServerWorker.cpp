#include "ISTcpServerWorker.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISConstants.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISTcpServerWorker::ISTcpServerWorker(QObject *parent)
	: ISTcpServerBase(parent),
	TcpApi(new ISTcpApi()),
	TcpSocket(nullptr)
{
	
}
//-----------------------------------------------------------------------------
ISTcpServerWorker::~ISTcpServerWorker()
{
	delete TcpApi;
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::incomingConnection(qintptr SocketDescriptor)
{
	ISTcpServerBase::incomingConnection(SocketDescriptor);
	TcpSocket = nextPendingConnection();
	connect(TcpSocket, &QTcpSocket::readyRead, this, &ISTcpServerWorker::ReadyRead);
	connect(TcpSocket, &QTcpSocket::disconnected, this, &ISTcpServerWorker::Disconnected);
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::ReadyRead()
{
	Buffer.append(TcpSocket->readAll());
	if (Buffer.right(CARAT_PACKET_SEPARATOR_SIZE) != CARAT_PACKET_SEPARATOR)
	{
		return;
	}

	//Удаляем сепаратор
	Buffer.remove(Buffer.size() - CARAT_PACKET_SEPARATOR_SIZE, CARAT_PACKET_SEPARATOR_SIZE);

	//Объект ответа
	ISTcpAnswer TcpAnswer;

	//Проверка валидности запроса
	QVariantMap VariantMap;
	QString ErrorString;
	if (ISTcp::IsValidQuery(Buffer, VariantMap, ErrorString))
	{
		QString QueryType = VariantMap["Type"].toString();
		if (TcpApi->Has(QueryType))
		{
			TcpApi->Execute(QueryType, VariantMap["Parameters"].toMap(), TcpAnswer);
		}
		else
		{
			TcpAnswer.SetError(QString("Invalid query \"%1\"").arg(QueryType));
		}
	}
	else
	{
		TcpAnswer.SetError(ErrorString);
	}
	Send(TcpSocket, TcpAnswer);
	Buffer.clear();
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::Disconnected()
{
	TcpSocket->deleteLater();
	close();
	QCoreApplication::quit();
}
//-----------------------------------------------------------------------------
