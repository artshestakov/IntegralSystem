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
	WorkerCount(0)
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
	ISLOGGER_I(__CLASS__, "Starting...");

	//Получаем и проверяем параметр порта сервера
	unsigned short tcp_port = CONFIG_INT(CONST_CONFIG_TCPSERVER_PORT);
	if (tcp_port < 1 || tcp_port >= USHRT_MAX) //Если значение не входит в диапазон портов - использует порт по умолчанию
	{
		ISLOGGER_W(__CLASS__, QString("Invalid config value %1: %2. The default port will be used: %3.").arg(CONST_CONFIG_TCPSERVER_PORT).arg(tcp_port).arg(CARAT_TCP_PORT));
		tcp_port = CARAT_TCP_PORT;
	}

	//Получаем и проверяем значение параметра количества воркеров
	WorkerCount = CONFIG_INT(CONST_CONFIG_TCPSERVER_WORKERS);
	if (WorkerCount < 1 || WorkerCount >= ULONG_MAX)
	{
		unsigned int worker_count = std::thread::hardware_concurrency();
		ISLOGGER_W(__CLASS__, QString("Invalid config value %1: %2. Will use %3 workers by default.").arg(CONST_CONFIG_TCPSERVER_WORKERS).arg(WorkerCount).arg(worker_count));
		WorkerCount = worker_count;
	}
	Workers.resize(WorkerCount); //Устанавливаем размер вектору воркеров

	QString DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	int DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	QString DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	QString DBUser = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	QString DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);
	
	//Запуск потоков
	QEventLoop EventLoop;
	for (unsigned int i = 0; i < WorkerCount; ++i)
	{
		QThread *Thread = new QThread();
		ISTcpWorker *TcpWorker = new ISTcpWorker(DBHost, DBPort, DBName, DBUser, DBPassword);
		connect(TcpWorker, &ISTcpWorker::Answer, this, &ISTcpServer::SendAnswer, Qt::QueuedConnection);
		connect(TcpWorker, &ISTcpWorker::Started, &EventLoop, &QEventLoop::quit);
		Workers[i] = TcpWorker;

		connect(Thread, &QThread::started, TcpWorker, &ISTcpWorker::Run); //Запуск воркера

		TcpWorker->moveToThread(Thread); //Перемещаем воркер в отдельный поток
		Thread->start(); //Запускаем поток
		EventLoop.exec(); //Ожидаем запуска воркера

		//Отключаем сигнал от текущего воркера (на всякий случай)
		disconnect(TcpWorker, &ISTcpWorker::Started, &EventLoop, &QEventLoop::quit);
	}

	//Запускаем балансировщик
	if (!QtConcurrent::run(this, &ISTcpServer::QueueBalancerMessage).isStarted())
	{
		ErrorString = "Error starting QueueBalancerMessage";
		return false;
	}

	//Запуск TCP-сервера
	if (!listen(QHostAddress::AnyIPv4, tcp_port))
	{
		ErrorString = errorString();
		return false;
	}
	connect(this, &QTcpServer::acceptError, this, &ISTcpServer::AcceptError);
	ISLOGGER_I(__CLASS__, QString("Started. Port: %1. Workers: %2").arg(CARAT_TCP_PORT).arg(WorkerCount));
	return true;
}
//-----------------------------------------------------------------------------
void ISTcpServer::incomingConnection(qintptr SocketDescriptor)
{
	//Создаём сокет и подключаем все нобходимые сигналы
	ISTcpSocket *TcpSocket = new ISTcpSocket(SocketDescriptor, this);
	connect(TcpSocket, &ISTcpSocket::disconnected, this, &ISTcpServer::ClientDisconnected);
	ISLOGGER_I(__CLASS__, QString("New connect from ") + TcpSocket->peerAddress().toString());
}
//-----------------------------------------------------------------------------
void ISTcpServer::ClientDisconnected()
{
	ISTcpSocket *TcpSocket = dynamic_cast<ISTcpSocket*>(sender());
	ISLOGGER_I(__CLASS__, "Disconnected " + TcpSocket->peerAddress().toString());
	QTimer::singleShot(5000, TcpSocket, &ISTcpSocket::deleteLater);
}
//-----------------------------------------------------------------------------
void ISTcpServer::AcceptError(QTcpSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
	ISLOGGER_E(__CLASS__, errorString());
}
//-----------------------------------------------------------------------------
void ISTcpServer::QueueBalancerMessage()
{
	ISTcpMessage *TcpMessage = nullptr;
	while (true)
	{
		ISSleep(1);
		TcpMessage = ISTcpQueue::Instance().GetMessage();
		if (TcpMessage) //Если есть сообщение на очереди - ищем свободный воркер
		{
			unsigned int Index = 0;
			while (Index < WorkerCount)
			{
				ISTcpWorker *TcpWorker = Workers[Index]; //Получаем текущий воркер
				if (!TcpWorker->GetRunning()) //Если он не занят - пердаём ему очередное сообщение и выходим из цикла
				{
					TcpWorker->SetMessage(TcpMessage);
					TcpMessage = nullptr;
					break;
				}
				++Index; //Инкрементируем индекс
				if (Index == WorkerCount) //Если текущий индекс сравняелся с количеством воркером - обнуляем его
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
	if (TcpSocket->state() == ISTcpSocket::ConnectedState) //Если сокет не отключался
	{
		//Записываем в него ответ и ждём отправки
		TcpSocket->write(TcpAnswer->ToByteArray());
		while (TcpSocket->state() == ISTcpSocket::ConnectedState)
		{
			if (TcpSocket->bytesToWrite() > 0) //Если есть баты доступные для записи - пишем
			{
				ISSystem::ProcessEvents();
				ISSleep(1);
			}
			else //Записали все - выходим из цикла
			{
				break;
			}
		}
	}
	delete TcpAnswer; //Удаляем указатель на объект ответа
}
//-----------------------------------------------------------------------------
