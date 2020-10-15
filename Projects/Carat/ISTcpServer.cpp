#include "ISTcpServer.h"
#include "ISLogger.h"
#include "ISConfig.h"
#include "ISAlgorithm.h"
#include "ISTcpQueue.h"
#include "ISTcp.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISTcpServer::ISTcpServer(quint16 tcp_port, unsigned int worker_count)
	: QTcpServer(),
	ErrorString(NO_ERROR_STRING),
	TcpPort(tcp_port),
	WorkerCount(worker_count),
	Workers(std::vector<ISTcpWorker *>(WorkerCount))
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
bool ISTcpServer::Run()
{
	QString DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	int DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	QString DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	QString DBUser = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	QString DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);
	
	//������ �������
	QEventLoop EventLoop;
	for (unsigned int i = 0; i < WorkerCount; ++i)
	{
		QThread *Thread = new QThread();
		ISTcpWorker *TcpWorker = new ISTcpWorker(DBHost, DBPort, DBName, DBUser, DBPassword);
		connect(TcpWorker, &ISTcpWorker::Answer, this, &ISTcpServer::SendAnswer, Qt::QueuedConnection);
		connect(TcpWorker, &ISTcpWorker::Started, &EventLoop, &QEventLoop::quit);
		Workers[i] = TcpWorker;

		connect(Thread, &QThread::started, TcpWorker, &ISTcpWorker::Run); //������ �������

		TcpWorker->moveToThread(Thread); //���������� ������ � ��������� �����
		Thread->start(); //��������� �����
		EventLoop.exec(); //������� ������� �������
		disconnect(TcpWorker, &ISTcpWorker::Started, &EventLoop, &QEventLoop::quit); //��������� ������ �� �������� ������� (�� ������ ������)
	}

	//��������� ��������������
	if (!QtConcurrent::run(this, &ISTcpServer::QueueBalancerMessage).isStarted())
	{
		ErrorString = "Error starting QueueBalancerMessage";
		return false;
	}

	//������ TCP-�������
	if (!listen(QHostAddress::AnyIPv4, TcpPort))
	{
		ErrorString = errorString();
		return false;
	}
	connect(this, &QTcpServer::acceptError, this, &ISTcpServer::AcceptError);
	return true;
}
//-----------------------------------------------------------------------------
void ISTcpServer::incomingConnection(qintptr SocketDescriptor)
{
	//������ ����� � ��������� ��� �� ���������� �������
	ISTcpSocket *TcpSocket = new ISTcpSocket(SocketDescriptor, this);
	addPendingConnection(TcpSocket);
	ISLOGGER_I(QString("Incoming connection from ") + TcpSocket->peerAddress().toString());

	//��������� ����� � ������ � ���������� �������
	Clients.emplace_back(TcpSocket);
	connect(TcpSocket, &ISTcpSocket::disconnected, this, &ISTcpServer::ClientDisconnected);
	connect(TcpSocket, static_cast<void(ISTcpSocket::*)(QAbstractSocket::SocketError)>(&ISTcpSocket::error), this, &ISTcpServer::ClientError);
}
//-----------------------------------------------------------------------------
void ISTcpServer::ClientDisconnected()
{
	ISTcpSocket *TcpSocket = dynamic_cast<ISTcpSocket*>(sender());
	ISLOGGER_I("Disconnected " + TcpSocket->peerAddress().toString());
    if (ISAlgorithm::VectorTake(Clients, TcpSocket)) //���� ����� ����� � ������ � ������� ��������� ��� �� ���� - �������� �������������� �������� ��������� �� ����
	{
		QTimer::singleShot(5000, TcpSocket, &ISTcpSocket::deleteLater);
	}
	else
	{
		ISLOGGER_W("Not found client");
	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::ClientError(QAbstractSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
}
//-----------------------------------------------------------------------------
void ISTcpServer::AcceptError(QTcpSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
}
//-----------------------------------------------------------------------------
void ISTcpServer::QueueBalancerMessage()
{
	ISTcpMessage *TcpMessage = nullptr;
	while (true)
	{
		ISSleep(1);
		TcpMessage = ISTcpQueue::Instance().GetMessage();
		if (TcpMessage) //���� ���� ��������� �� ������� - ���� ��������� ������
		{
			unsigned int Index = 0;
			while (Index < WorkerCount)
			{
				ISTcpWorker *TcpWorker = Workers[Index]; //�������� ������� ������
				if (!TcpWorker->GetRunning()) //���� �� �� ����� - ������ ��� ��������� ��������� � ������� �� �����
				{
					TcpWorker->SetMessage(TcpMessage);
					TcpMessage = nullptr;
					break;
				}
				++Index; //�������������� ������
				if (Index == WorkerCount) //���� ������� ������ ���������� � ����������� �������� - �������� ���
				{
					Index = 0;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::SendAnswer(ISTcpAnswer *TcpAnswer)
{
	ISTcpSocket *TcpSocket = TcpAnswer->GetSocket();
	if (TcpSocket->state() == ISTcpSocket::ConnectedState) //���� ����� �� ����������
	{
		//���������� � ���� ����� � ��� ��������
		TcpSocket->write(TcpAnswer->ToByteArray());
		while (TcpSocket->state() == ISTcpSocket::ConnectedState)
		{
			if (TcpSocket->bytesToWrite() > 0)
			{
				ISSystem::ProcessEvents();
				ISSleep(1);
			}
		}
	}
	delete TcpAnswer; //������� ��������� �� ������ ������
}
//-----------------------------------------------------------------------------
