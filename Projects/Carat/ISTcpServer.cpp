#include "ISTcpServer.h"
#include "ISLogger.h"
#include "ISConfig.h"
#include "ISAlgorithm.h"
#include "ISTcpQueue.h"
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
	
	//Получение максимального количества потоков и их запуск
	for (unsigned int i = 0; i < WorkerCount; ++i)
	{
		QThread *Thread = new QThread();
		ISTcpWorker *TcpWorker = new ISTcpWorker(DBHost, DBPort, DBName, DBUser, DBPassword);
		Workers[i] = TcpWorker;

		connect(Thread, &QThread::started, TcpWorker, &ISTcpWorker::Run); //Запуск воркера
		TcpWorker->moveToThread(Thread);
		Thread->start();
	}

	//Запускаем балансировщик очереди
	QtConcurrent::run(this, &ISTcpServer::QueueBalancer);

	//Запуск TCP-сервера
	bool Result = listen(QHostAddress::AnyIPv4, TcpPort);
	if (Result)
	{
		connect(this, &QTcpServer::acceptError, this, &ISTcpServer::AcceptError);
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
	//Создаём сокет и добавляем его во внутреннюю очередь
	ISTcpSocket *TcpSocket = new ISTcpSocket(SocketDescriptor, this);
	addPendingConnection(TcpSocket);
	ISLOGGER_I(QString("Incoming connection from ") + TcpSocket->peerAddress().toString());

	//Добавляем сокет в список и подключаем сигналы
	Clients.emplace_back(TcpSocket);
	connect(TcpSocket, &ISTcpSocket::disconnected, this, &ISTcpServer::ClientDisconnected);
	connect(TcpSocket, static_cast<void(ISTcpSocket::*)(QAbstractSocket::SocketError)>(&ISTcpSocket::error), this, &ISTcpServer::ClientError);
}
//-----------------------------------------------------------------------------
void ISTcpServer::ClientDisconnected()
{
	ISTcpSocket *TcpSocket = dynamic_cast<ISTcpSocket*>(sender());
	ISLOGGER_I("Disconnected " + TcpSocket->peerAddress().toString());
    if (ISAlgorithm::VectorTake(Clients, TcpSocket))
	{
        TcpSocket->deleteLater(); //Вызываем отложенное удаление указателя на QTcpSocket
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
void ISTcpServer::QueueBalancer()
{
	ISTcpMessage *TcpMessage = nullptr;
	while (true)
	{
		//Спим и даём возможность поработать другим потокам
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
