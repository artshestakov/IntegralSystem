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
#include "ISNetwork.h"
#include "ISAes256.h"
#include "ISTrace.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QS_KEYS = PREPARE_QUERY("SELECT usename, md5(usename || :Port) || right(passwd, length(passwd) - 3) AS Keys "
									   "FROM pg_shadow "
									   "WHERE passwd IS NOT NULL "
									   "ORDER BY usename");
//-----------------------------------------------------------------------------
static QString QS_AUTH = PREPARE_QUERY("SELECT "
									   "(SELECT COUNT(*) FROM _users WHERE usrs_login = :Login), "
									   "(right(passwd, length(passwd) - 3) = md5(:Password || :Login))::BOOLEAN AS password, "
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
	ISTRACE();
	ISTcpServerBase::incomingConnection(SocketDescriptor);
	QTcpSocket *TcpSocket = nextPendingConnection();
	if (TcpSocket)
	{
		ISLOGGER_I(QString("Incoming auth from ") + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
		connect(TcpSocket, &QTcpSocket::disconnected, TcpSocket, &QTcpSocket::deleteLater);
	}
	else
	{
		ISLOGGER_E("nextPendingConnection return null QTcpSocket");
		return;
	}

	QByteArray ByteArray;
	long Size = 0;

	while (true) //Ждём пока не придёт запрос
	{
		ISSleep(50);
		ISSystem::ProcessEvents();
		if (TcpSocket->bytesAvailable() > 0) //Если есть данные, которые можно прочитать
		{
			ByteArray.append(TcpSocket->readAll()); //Читаем данные
			if (!Size) //Размеры ещё не известены - вытаскиваем их
			{
				Size = ISTcp::GetQuerySizeFromBuffer(ByteArray);
			}

			if (!Size) //Если размер не удалось вытащить - вероятно пришли невалидные данные - отключаем клиента
			{
				ISLOGGER_E("Not getting query size. Disconnecting client " + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()) );
				TcpSocket->abort();
				return;
			}

			if (ByteArray.size() == Size) //Запрос пришёл полностью - выходим из цикла
			{
				break;
			}
		}
	}

	//Запрашиваем все ключи
	ISQuery qSelectKeys(QS_KEYS);
	qSelectKeys.BindValue(":Port", TcpSocket->peerPort());
	if (!qSelectKeys.Execute()) //Ошибка запроса - отключаем клиента
	{
		ISLOGGER_E("Not getting keys: " + qSelectKeys.GetErrorString());
		TcpSocket->abort();
		return;
	}

	QString Login;
	QVariantMap VariantMap;
	while (qSelectKeys.Next()) //Перебираем ключи
	{
		//Получаем очередной ключ
		std::string Key = qSelectKeys.ReadColumn("Keys").toString().toStdString();
		std::vector<unsigned char> VectorKey(Key.begin(), Key.end());

		std::vector<unsigned char> Vector;
		ISAes256::decrypt(VectorKey, std::vector<unsigned char>(ByteArray.begin(), ByteArray.end()), Vector);

		//Проверка валидности запроса
		QString ErrorString;
		if (ISTcp::IsValidQuery(QString::fromStdString(std::string(Vector.begin(), Vector.end())).toUtf8(), VariantMap, ErrorString)) //Валидация прошла успешно
		{
			Login = qSelectKeys.ReadColumn("usename").toString();
			break;
		}
	}

	if (Login.isEmpty()) //Если не удалось подобрать ключ
	{
		SendError(TcpSocket, "Failed to find decryption key");
		return;
	}

	//Если поле с типом запроса отсутствует
	if (!VariantMap.contains("Type"))
	{
		SendError(TcpSocket, "Not found field \"Type\"");
		return;
	}
	
	//Если поле с типом запроса пустое
	QString QueryType = VariantMap["Type"].toString();
	if (QueryType.isEmpty())
	{
		SendError(TcpSocket, "Query type not specified");
		return;
	}

	//Проверка типа запроса, если не авторизация - ошибка
	if (QueryType != API_AUTH)
	{
		SendError(TcpSocket, QString("Invalid query type \"%1\"").arg(QueryType));
		return;
	}

	VariantMap = VariantMap["Parameters"].toMap();

	if (!VariantMap.contains("Password")) //Если поле с паролем отсутствует
	{
		SendError(TcpSocket, "Not found field \"Password\"");
		return;
	}

	QString Password = VariantMap["Password"].toString();
	if (Password.isEmpty()) //Если поле с паролем пустое
	{
		SendError(TcpSocket, "Field \"Password\" is empty");
		return;
	}

	//Проверка пользователя
	ISQuery qSelectAuth(QS_AUTH);
	qSelectAuth.BindValue(":Login", Login);
	qSelectAuth.BindValue(":Password", Password);

	//Если такой логин в БД не существует
	if (!qSelectAuth.ExecuteFirst() && !qSelectAuth.GetCountResultRows())
	{
		SendError(TcpSocket, "Message.Error.LoginNotExist");
		return;
	}

	//Если пароль неправильный
	if (!qSelectAuth.ReadColumn("password").toBool())
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

	ISLOGGER_I("Auth is done");

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
