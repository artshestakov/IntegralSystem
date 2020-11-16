#include "ISTcpServer.h"
#include "ISLogger.h"
#include "ISConfig.h"
#include "ISAlgorithm.h"
#include "ISTcpQueue.h"
#include "ISTcp.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISTcpServer::ISTcpServer(QObject *parent)
	: QTcpServer(parent),
	ErrorString(NO_ERROR_STRING),
	WorkerCount(0),
	BalancerRunning(false),
	BalancerFinished(false)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpServer::~ISTcpServer()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpServer::Run()
{
	ISLOGGER_I(__CLASS__, "Starting...");

	//�������� � ��������� �������� ����� �������
	unsigned short tcp_port = CONFIG_INT(CONST_CONFIG_TCPSERVER_PORT);
	if (tcp_port < 1 || tcp_port >= USHRT_MAX) //���� �������� �� ������ � �������� ������ - ���������� ���� �� ���������
	{
		ISLOGGER_W(__CLASS__, QString("Invalid config value %1: %2. The default port will be used: %3.").arg(CONST_CONFIG_TCPSERVER_PORT).arg(tcp_port).arg(CARAT_TCP_PORT));
		tcp_port = CARAT_TCP_PORT;
	}

	//�������� � ��������� �������� ��������� ���������� ��������
	WorkerCount = CONFIG_INT(CONST_CONFIG_TCPSERVER_WORKERS);
	if (WorkerCount < 1 || WorkerCount >= ULONG_MAX)
	{
		unsigned int worker_count = std::thread::hardware_concurrency();
		ISLOGGER_W(__CLASS__, QString("Invalid config value %1: %2. Will use %3 workers by default.").arg(CONST_CONFIG_TCPSERVER_WORKERS).arg(WorkerCount).arg(worker_count));
		WorkerCount = worker_count;
	}
	Workers.resize(WorkerCount); //������������� ������ ������� ��������

	QString DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	int DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	QString DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	QString DBUser = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	QString DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);
	
	ISConfigurationInfo ConfigurationInfo;
	if (!GetConfigurationInfo(CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION), ConfigurationInfo))
	{
		ISLOGGER_E(__CLASS__, "Not read configuration: " + ErrorString);
		return false;
	}

	//������ �������
	QEventLoop EventLoop;
	for (unsigned int i = 0; i < WorkerCount; ++i)
	{
		QThread *Thread = new QThread();
		ISTcpWorker *TcpWorker = new ISTcpWorker(DBHost, DBPort, DBName, DBUser, DBPassword, ConfigurationInfo);
		connect(TcpWorker, &ISTcpWorker::Answer, this, &ISTcpServer::SendAnswer, Qt::QueuedConnection);
		connect(TcpWorker, &ISTcpWorker::StartedDone, &EventLoop, &QEventLoop::quit);
		connect(TcpWorker, &ISTcpWorker::StartedFailed, &EventLoop, &QEventLoop::quit);
		Workers[i] = TcpWorker;

		connect(Thread, &QThread::started, TcpWorker, &ISTcpWorker::Run); //������ �������

		TcpWorker->moveToThread(Thread); //���������� ������ � ��������� �����
		Thread->start(); //��������� �����
		EventLoop.exec(); //������� ������� �������

		//��������� ������� �� �������� ������� (�� ������ ������)
		disconnect(TcpWorker, &ISTcpWorker::StartedDone, &EventLoop, &QEventLoop::quit);
		disconnect(TcpWorker, &ISTcpWorker::StartedFailed, &EventLoop, &QEventLoop::quit);

		if (!TcpWorker->GetStarted())
		{
			return false;
		}
	}

	//�������������� �������
	ISTcpQueue::Instance().ReadMessageID();

	//��������� �������������
	if (!QtConcurrent::run(this, &ISTcpServer::QueueBalancerMessage).isStarted())
	{
        ISLOGGER_E(__CLASS__, "Not starting QueueBalancerMessage");
		return false;
	}
	BalancerRunning = true;

	//������ TCP-�������
	if (!listen(QHostAddress::AnyIPv4, tcp_port))
	{
        ISLOGGER_E(__CLASS__, QString("Not listen port %1: %2").arg(tcp_port).arg(errorString()));
		return false;
	}
	connect(this, &QTcpServer::acceptError, this, &ISTcpServer::AcceptError);
	ISLOGGER_I(__CLASS__, QString("Started. Port: %1. Workers: %2").arg(CARAT_TCP_PORT).arg(WorkerCount));
	return true;
}
//-----------------------------------------------------------------------------
void ISTcpServer::Stop()
{
	//������� ������������� TCP-������, ��� ����� �������� ����� �������� �����������
	ISLOGGER_I(__CLASS__, "Stopping");
	close();

	//������������� �������
	ISTcpQueue::Instance().Shutdown();

	//������������� �������� � ��� ���� �� �� �����������
	CRITICAL_SECTION_LOCK(&CriticalSection);
	BalancerRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	while (!BalancerFinished)
	{
		ISSleep(10);
	}

	//������� ������� ������ � ������������� ���
	for (unsigned int i = 0; i < WorkerCount; ++i)
	{
		ISTcpWorker *TcpWorker = Workers[i]; //�������� ������� ������
		TcpWorker->Stop(); //������������� ���
		TcpWorker->thread()->quit(); //�������� ����� ������� � ���� ��� �������������
		connect(TcpWorker->thread(), &QThread::finished, TcpWorker, &ISTcpWorker::deleteLater); //����� ��������� ������ �������� � ������ �������
	}

	//���������� ������� ������������� � ����
	ISTcpQueue::Instance().WriteMessageID();
	ISLOGGER_I(__CLASS__, "Stopped");
}
//-----------------------------------------------------------------------------
void ISTcpServer::incomingConnection(qintptr SocketDescriptor)
{
	//������ ����� � ���������� ��� ���������� �������
	ISTcpSocket *TcpSocket = new ISTcpSocket(SocketDescriptor, this);
	connect(TcpSocket, &ISTcpSocket::disconnected, this, &ISTcpServer::ClientDisconnected, Qt::QueuedConnection);
	ISLOGGER_I(__CLASS__, "Connect " + TcpSocket->peerAddress().toString());
}
//-----------------------------------------------------------------------------
void ISTcpServer::ClientDisconnected()
{
	ISTcpSocket *TcpSocket = dynamic_cast<ISTcpSocket*>(sender());
	ISLOGGER_I(__CLASS__, "Disconnect " + TcpSocket->GetAddress());
	QTimer::singleShot(1000, TcpSocket, &ISTcpSocket::deleteLater); //��������� �������� ������ ����� �������
}
//-----------------------------------------------------------------------------
void ISTcpServer::AcceptError(QTcpSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
	ISLOGGER_C(__CLASS__, "Accept error: " + errorString());
}
//-----------------------------------------------------------------------------
void ISTcpServer::QueueBalancerMessage()
{
	ISTcpMessage *TcpMessage = nullptr;
	while (true)
	{
		//���������, ������� �� ��������
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool running_balancer = BalancerRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (!running_balancer)
		{
			break;
		}

		ISSleep(1);
		TcpMessage = ISTcpQueue::Instance().GetMessage();
		if (TcpMessage) //���� ���� ��������� �� ������� - ���� ��������� ������
		{
			unsigned int Index = 0;
			while (Index < WorkerCount)
			{
				ISTcpWorker *TcpWorker = Workers[Index]; //�������� ������� ������
				if (!TcpWorker->GetRunning()) //���� �� �� ����� - ������� ��� ��������� ��������� � ������� �� �����
				{
					TcpWorker->SetMessage(TcpMessage);
					TcpMessage = nullptr;
					break;
				}
				++Index; //�������������� ������
				if (Index == WorkerCount) //���� ������� ������ ��������� � ����������� �������� - �������� ���
				{
					Index = 0;
				}
			}
		}
	}
	BalancerFinished = true;
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
			if (TcpSocket->bytesToWrite() > 0) //���� ���� ���� ��������� ��� ������ - �����
			{
				PROCESS_EVENTS();
				ISSleep(1);
			}
			else //�������� ��� - ������� �� �����
			{
				break;
			}
		}
	}
	delete TcpAnswer; //������� ��������� �� ������ ������
}
//-----------------------------------------------------------------------------
bool ISTcpServer::GetConfigurationInfo(const QString &ConfigurationName, ISConfigurationInfo &ConfigurationInfo)
{
	QFile File(PATH_CONFIGURATIONS_SCHEME);
	bool Result = File.open(QIODevice::ReadOnly);
	if (Result)
	{
		QString Content = File.readAll();
		File.close();

		QDomElement DomElement = ISSystem::GetDomElement(Content);
		QDomNode DomNode = DomElement.firstChild();
		while (!DomNode.isNull())
		{
			QDomNamedNodeMap DomNamedNodeMap = DomNode.attributes();
			QString configuration_name = DomNamedNodeMap.namedItem("Name").nodeValue();
			if (configuration_name == ConfigurationName)
			{
				ConfigurationInfo.UID = DomNamedNodeMap.namedItem("UID").nodeValue();
				ConfigurationInfo.Name = configuration_name;
				ConfigurationInfo.LocalName = DomNamedNodeMap.namedItem("LocalName").nodeValue();
				ConfigurationInfo.DesktopForm = DomNamedNodeMap.namedItem("DesktopForm").nodeValue();
				ConfigurationInfo.DateExpired = QDate::fromString(DomNamedNodeMap.namedItem("DateExpired").nodeValue(), FORMAT_DATE_V2);
				ConfigurationInfo.LogoName = DomNamedNodeMap.namedItem("LogoName").nodeValue();
				return true;
			}
			DomNode = DomNode.nextSibling();
		}
		ErrorString = "Not found configuration";
	}
	else
	{
		ErrorString = QString("Error read file \"%1\": %2").arg(File.fileName()).arg(File.errorString());
	}
	return false;
}
//-----------------------------------------------------------------------------
