#include "ISTcpServer.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISDefinesCore.h"
#include "ISLogger.h"
#include "ISDatabase.h"
#include "ISConfig.h"
#include "ISNetwork.h"
#include "ISTrace.h"
#include "ISAlgorithm.h"
#include "ISLocalization.h"
#include "ISTcpQueue.h"
//-----------------------------------------------------------------------------
static QString QS_AUTH = PREPARE_QUERY("SELECT "
									   "usrs_issystem, "
									   "usrs_isdeleted, "
									   "usrs_group, "
									   "usrs_accessallowed, "
									   "usrs_accountlifetime, "
									   "usrs_accountlifetimestart, "
									   "usrs_accountlifetimeend "
									   "FROM _users "
									   "WHERE usrs_login = :Login");
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
	DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	
	//Получение максимального количества потоков и их запуск
	for (unsigned int i = 0; i < WorkerCount; ++i)
	{
		QThread *Thread = new QThread();
		ISTcpWorker *TcpWorker = new ISTcpWorker();
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
	return;

	QByteArray Buffer;
	

	

	//Проверка типа запроса, если не авторизация - ошибка
	//if (QueryType != API_AUTH)
	{
		//SendError(TcpSocket, LANG("CaratError.InvalidQueryType").arg(QueryType));
		return;
	}

	//VariantMap = VariantMap["Parameters"].toMap();
	//if (!VariantMap.contains("Login")) //Если поле с логином отсутствует
	{
		//SendError(TcpSocket, LANG("CaratError.NotFoundField").arg("Login"));
		return;
	}

	//QString Login = VariantMap["Login"].toString();
	//if (Login.isEmpty()) //Если поле с логином пустое
	{
		//SendError(TcpSocket, LANG("CaratError.FieldIsEmpty").arg("Login"));
		return;
	}

	//if (!VariantMap.contains("Password")) //Если поле с паролем отсутствует
	{
		//SendError(TcpSocket, LANG("CaratError.NotFoundField").arg("Password"));
		return;
	}

	//QString Password = VariantMap["Password"].toString();
	//if (Password.isEmpty()) //Если поле с паролем пустое
	{
		//SendError(TcpSocket, LANG("CaratError.FieldIsEmpty").arg("Password"));
		return;
	}

	//Проверка пользователя
	ISQuery qSelectAuth(QS_AUTH);
	qSelectAuth.BindValue(":Login", /*Login*/"");
	if (!qSelectAuth.ExecuteFirst())
	{
		//SendError(TcpSocket, LANG("CaratError.CheckingLogin"));
		return;
	}

	//Если такой логин помечен на удаление
	if (qSelectAuth.ReadColumn("usrs_isdeleted").toBool())
	{
		//SendError(TcpSocket, "Message.Error.CurrentUserIsDeleted");
		return;
	}

	//Если у пользователя нет права доступа
	if (!qSelectAuth.ReadColumn("usrs_accessallowed").toBool())
	{
		//SendError(TcpSocket, "Message.Error.User.NotAccessAllowed");
		return;
	}

	//Проверка наличия привязки пользователя к группе
	if (!qSelectAuth.ReadColumn("usrs_issystem").toBool()) //Если пользователь не системный - проверяем привязку
	{
		if (qSelectAuth.ReadColumn("usrs_group").toInt() == 0) //Привязка отсутствует
		{
			//SendError(TcpSocket, "Message.Error.User.NotLinkWithGroup");
			return;
		}
	}

	//Если для пользователя настроено ограничение срока действия учётной записи
	if (qSelectAuth.ReadColumn("usrs_accountlifetime").toBool())
	{
		QDate CurrentDate = QDate::currentDate();
		QDate DateStart = qSelectAuth.ReadColumn("usrs_accountlifetimestart").toDate();
		QDate DateEnd = qSelectAuth.ReadColumn("usrs_accountlifetimeend").toDate();
		if (CurrentDate < DateStart)
		{
			//SendError(TcpSocket, "Message.Warning.AccountLifetimeNotStarted");
			return;
		}
		else if (CurrentDate > DateEnd)
		{
			//SendError(TcpSocket, "Message.Warning.AccountLifetimeEnded");
			return;
		}
	}

	//Проверка соединения с БД по логину и паролю
	if (ISDatabase::Instance().Connect(CONNECTION_USER, DBHost, DBPort, DBName, /*Login*/"", /*Password*/""))
	{
		ISDatabase::Instance().Disconnect(CONNECTION_USER);
	}
	else //Если соединение к БД произошло с ошибкой
	{
		//SendError(TcpSocket, ISDatabase::Instance().GetErrorString());
		return;
	}

	ISLOGGER_I("Auth is done");
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
