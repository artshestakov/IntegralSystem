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
		connect(TcpWorker, &ISTcpWorker::StartedDone, &EventLoop, &QEventLoop::quit);
		connect(TcpWorker, &ISTcpWorker::StartedFailed, &EventLoop, &QEventLoop::quit);
		Workers[i] = TcpWorker;

		connect(Thread, &QThread::started, TcpWorker, &ISTcpWorker::Run); //Запуск воркера

		TcpWorker->moveToThread(Thread); //Перемещаем воркер в отдельный поток
		Thread->start(); //Запускаем поток
		EventLoop.exec(); //Ожидаем запуска воркера

		//Отключаем сигналы от текущего воркера (на всякий случай)
		disconnect(TcpWorker, &ISTcpWorker::StartedDone, &EventLoop, &QEventLoop::quit);
		disconnect(TcpWorker, &ISTcpWorker::StartedFailed, &EventLoop, &QEventLoop::quit);

		if (!TcpWorker->GetStarted())
		{
			return false;
		}
	}

	//Инициализируем очередь
	ISTcpQueue::Instance().ReadMessageID();

	//Запускаем балансировщик
	if (!QtConcurrent::run(this, &ISTcpServer::QueueBalancerMessage).isStarted())
	{
        ISLOGGER_E(__CLASS__, "Not starting QueueBalancerMessage");
		return false;
	}
	BalancerRunning = true;

	//Запуск TCP-сервера
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
	//Сначала останавливаем TCP-сервер, тем самым прерывая новые входящие подключения
	ISLOGGER_I(__CLASS__, "Stopping");
	close();

	//Останавливаем очередь
	ISTcpQueue::Instance().Shutdown();

	//Останавливаем балансер и ждём пока он не остановится
	CRITICAL_SECTION_LOCK(&CriticalSection);
	BalancerRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	while (!BalancerFinished)
	{
		ISSleep(10);
	}

	//Обходим кааждый воркер и останавливаем его
	for (unsigned int i = 0; i < WorkerCount; ++i)
	{
		ISTcpWorker *TcpWorker = Workers[i]; //Получаем текущий воркер
		TcpWorker->Stop(); //Останавливаем его
		TcpWorker->thread()->quit(); //Получаем поток воркера и тоже его останавливаем
		connect(TcpWorker->thread(), &QThread::finished, TcpWorker, &ISTcpWorker::deleteLater); //После остановки потока удалится и объект воркера
	}

	//Записываем текущий идентификатор в файл
	ISTcpQueue::Instance().WriteMessageID();
	ISLOGGER_I(__CLASS__, "Stopped");
}
//-----------------------------------------------------------------------------
void ISTcpServer::incomingConnection(qintptr SocketDescriptor)
{
	//Создаём сокет и подключаем все нобходимые сигналы
	ISTcpSocket *TcpSocket = new ISTcpSocket(SocketDescriptor, this);
	connect(TcpSocket, &ISTcpSocket::disconnected, this, &ISTcpServer::ClientDisconnected, Qt::QueuedConnection);
	ISLOGGER_I(__CLASS__, "Connect " + TcpSocket->peerAddress().toString());
}
//-----------------------------------------------------------------------------
void ISTcpServer::ClientDisconnected()
{
	ISTcpSocket *TcpSocket = dynamic_cast<ISTcpSocket*>(sender());
	ISLOGGER_I(__CLASS__, "Disconnect " + TcpSocket->peerAddress().toString());
	QTimer::singleShot(1000, TcpSocket, &ISTcpSocket::deleteLater); //Запускаем удаление сокета через секунду
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
		//Проверяем, запущен ли балансер
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool running_balancer = BalancerRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (!running_balancer)
		{
			break;
		}

		ISSleep(1);
		TcpMessage = ISTcpQueue::Instance().GetMessage();
		if (TcpMessage) //Если есть сообщение на очереди - ищем свободный воркер
		{
			unsigned int Index = 0;
			while (Index < WorkerCount)
			{
				ISTcpWorker *TcpWorker = Workers[Index]; //Получаем текущий воркер
				if (!TcpWorker->GetRunning()) //Если он не занят - передаём ему очередное сообщение и выходим из цикла
				{
					TcpWorker->SetMessage(TcpMessage);
					TcpMessage = nullptr;
					break;
				}
				++Index; //Инкрементируем индекс
				if (Index == WorkerCount) //Если текущий индекс сравнялся с количеством воркером - обнуляем его
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
