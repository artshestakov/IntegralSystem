#include "ISTcpServerCarat.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISDefinesCore.h"
#include "ISLogger.h"
#include "ISDatabase.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
static QString QS_AUTH = PREPARE_QUERY("SELECT "
									   "(SELECT COUNT(*) FROM _users WHERE usrs_login = :Login), "
									   "right(passwd, length(passwd) - 3) AS passwd, "
									   "usrs_issystem, "
									   "usrs_isdeleted, "
									   "(usrs_group != 0)::BOOLEAN AS usrs_group, "
									   "usrs_accessallowed, "
									   "usrs_accountlifetime, "
									   "usrs_accountlifetimestart, "
									   "usrs_accountlifetimeend "
									   "FROM _users "
									   "LEFT JOIN pg_shadow ON usename = usrs_login "
									   "WHERE usrs_login = :Login");
//-----------------------------------------------------------------------------
ISTcpServerCarat::ISTcpServerCarat(QObject *parent)
	: ISTcpServerBase(parent),
	ServerController(nullptr)
{
	
}
//-----------------------------------------------------------------------------
ISTcpServerCarat::~ISTcpServerCarat()
{
	
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::Run(quint16 Port)
{
	//Запуск локального сервера для контроля воркеров
	ServerController = new QLocalServer(this);
	ServerController->setMaxPendingConnections(1);
	bool Result = ServerController->listen(CARAT_CONTROLLER_PORT);
	if (Result)
	{
		Result = ISTcpServerBase::Run(Port);
	}
	else
	{
		SetErrorString(ServerController->errorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::incomingConnection(qintptr SocketDescriptor)
{
	ISTcpServerBase::incomingConnection(SocketDescriptor);
	QTcpSocket *TcpSocket = nextPendingConnection();
	if (!TcpSocket)
	{
		ISLOGGER_E("nextPendingConnection return null QTcpSocket");
	}
	connect(TcpSocket, &QTcpSocket::disconnected, TcpSocket, &QTcpSocket::deleteLater);

	QByteArray ByteArray;
	long Size = 0;

	while (true) //Ждём пока не придёт запрос
	{
		//Если запрос так и не пришёл - отключаем клиента
		if (!TcpSocket->waitForReadyRead(CARAT_TIMEOUT_INCOMING_QUERY))
		{
			SendError(TcpSocket, "Query not received");
			return;
		}

		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (!Size) //Размер ещё не известен - вытаскиваем его
			{
				Size = ISTcp::GetQuerySizeFromBuffer(ByteArray);
			}

			if (!Size) //Если размер не удалось вытащить - вероятно пришли невалидные данные - отключаем клиента
			{
				SendError(TcpSocket, "Query is not a valid");
				return;
			}

			if (ByteArray.size() == Size) //Запрос пришёл полностью - выходим из цикла
			{
				break;
			}
		}
	}
	
	//Проверка валидности запроса
	QVariantMap VariantMap;
	QString ErrorString;
	if (!ISTcp::IsValidQuery(ByteArray, VariantMap, ErrorString))
	{
		SendError(TcpSocket, ErrorString);
		return;
	}

	//Проверка типа запроса
	QString QueryType = VariantMap["Type"].toString();
	if (QueryType != API_AUTH) //Если не авторизация - ошибка
	{
		SendError(TcpSocket, QString("Invalid query type \"%1\"").arg(QueryType));
		return;
	}

	VariantMap = VariantMap["Parameters"].toMap();

	if (!VariantMap.contains("Login")) //Если поле с логином отсутствует
	{
		SendError(TcpSocket, "Not found field \"Login\"");
		return;
	}

	if (!VariantMap.contains("Password")) //Если поле с паролем отсутствует
	{
		SendError(TcpSocket, "Not found field \"Password\"");
		return;
	}

	QString Login = VariantMap["Login"].toString();
	QString Password = VariantMap["Password"].toString();

	if (Login.isEmpty()) //Если поле с логином пустое
	{
		SendError(TcpSocket, "Field \"Login\" is empty");
		return;
	}

	if (Password.isEmpty()) //Если поле с паролем пустое
	{
		SendError(TcpSocket, "Field \"Password\" is empty");
		return;
	}

	//Проверка введенных данных пользователем
	ISQuery qSelectAuth(QS_AUTH);
	qSelectAuth.BindValue(":Login", Login);

	//Если такой логин в БД не существует
	if (!qSelectAuth.ExecuteFirst() && !qSelectAuth.GetCountResultRows())
	{
		SendError(TcpSocket, "Message.Error.LoginNotExist");
		return;
	}

	//Если пароль неправильный
	if (qSelectAuth.ReadColumn("passwd").toString() != ISSystem::StringToMD5(Password + Login))
	{
		SendError(TcpSocket, "Message.Error.InvalidPassword");
		return;
	}

	//Если такой логин помечен на удаление
	if (qSelectAuth.ReadColumn("usrs_isdeleted").toBool())
	{
		SendError(TcpSocket, "Message.Error.CurrentUserIsDeleted");
		return;
	}

	//Если у пользователя нет права доступа
	if (!qSelectAuth.ReadColumn("usrs_accessallowed").toBool())
	{
		SendError(TcpSocket, "Message.Error.User.NotAccessAllowed");
		return;
	}

	//Проверка наличия привязки пользователя к группе
	if (!qSelectAuth.ReadColumn("usrs_issystem").toBool()) //Если пользователь не системный - проверяем привязку
	{
		if (!qSelectAuth.ReadColumn("usrs_group").toBool()) //Привязка отсутствует
		{
			SendError(TcpSocket, "Message.Error.User.NotLinkWithGroup");
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
			SendError(TcpSocket, "Message.Warning.AccountLifetimeNotStarted");
			return;
		}
		else if (CurrentDate > DateEnd)
		{
			SendError(TcpSocket, "Message.Warning.AccountLifetimeEnded");
			return;
		}
	}
	
	//Проверка соединения с БД по логину и паролю
	if (ISDatabase::Instance().Connect(CONNECTION_USER, CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE), Login, Password))
	{
		ISDatabase::Instance().Disconnect(CONNECTION_USER);
	}
	else //Если соединение к БД произошло с ошибкой
	{
		SendError(TcpSocket, ISDatabase::Instance().GetErrorString());
		return;
	}

	//Ищем свободный порт
	QTcpServer TcpServer;
	quint16 Port = serverPort() + 1;
    for (; Port < USHRT_MAX; ++Port)
	{
		if (TcpServer.listen(QHostAddress::AnyIPv4, Port)) //Если удалось захватить порт - закрываем его и выходим из цикла
		{
			TcpServer.close();
			break;
		}
	}

	//Запуск воркера
	QString StringPort = QString::number(Port);
	bool Result = QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_DIR + "/CaratWorker" + EXTENSION_BINARY, QStringList() << StringPort << Login << Password, ISDefines::Core::PATH_APPLICATION_DIR);
	if (Result)
	{
		Result = ServerController->waitForNewConnection(CARAT_TIMEOUT_STARTED_WORKER); //Ожидаем подтверждение запуска от воркера
	}
	
	if (!Result) //Не удалось запустить воркер
	{
		SendError(TcpSocket, "Message.Error.StartedWorker");
		return;
	}

	//Формируем ответ с портом и отправляем его
	ISTcpAnswer TcpAnswer;
	TcpAnswer["Port"] = StringPort;
	Send(TcpSocket, TcpAnswer);
}
//-----------------------------------------------------------------------------
