#include "ISTcpServer.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISTcpServer::ISTcpServer(QObject *parent)
	: QTcpServer(parent),
	ErrorString(NO_ERROR_STRING),
	TcpApi(new ISTcpApi())
{
	
}
//-----------------------------------------------------------------------------
ISTcpServer::~ISTcpServer()
{
	delete TcpApi;
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

	//��� ���� �� ����� ������
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

	//�������� ���� �������
	QString QueryType = VariantMap["Type"].toString();
	if (QueryType != API_AUTH) //���� �� ����������� - ������
	{
		SendError(TcpSocket, QString("Invalid query type \"%1\"").arg(QueryType));
		return;
	}

	ISTcpAnswer TcpAnswer;
	TcpApi->Execute(API_AUTH, VariantMap["Parameters"].toMap(), TcpAnswer);
	if (TcpAnswer.IsError()) //������ �����������
	{
		SendError(TcpSocket, TcpAnswer.GetErrorString());
	}
	else //����������� ������ �������
	{

	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::AcceptError(QAbstractSocket::SocketError socket_error)
{
	ErrorString = errorString();
}
//-----------------------------------------------------------------------------
void ISTcpServer::SendError(QTcpSocket *TcpSocket, const QString &ErrorString)
{
	//��������� ����� � �������
	ISTcpAnswer TcpAnswer;
	TcpAnswer.SetError(ErrorString);

	//����������
	Send(TcpSocket, TcpAnswer);
	TcpSocket->abort();
}
//-----------------------------------------------------------------------------
void ISTcpServer::Send(QTcpSocket *TcpSocket, const QVariantMap &Data)
{
	//�������� ������
	TcpSocket->write(ISSystem::VariantMapToJsonString(Data).simplified().toUtf8() + CARAT_PACKET_SEPARATOR);
	TcpSocket->flush();

	//��� ���� ������ �����
	TcpSocket->waitForBytesWritten();
}
//-----------------------------------------------------------------------------
