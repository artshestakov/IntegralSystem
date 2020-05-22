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
ISTcpServerCarat::ISTcpServerCarat(QObject *parent)
	: ISTcpServerBase(parent),
	ServerController(nullptr),
	IsDisconnected(false),
	HModuleCrypter(NULL),
	decrypt_message(NULL),
	get_error(NULL)
{
	
}
//-----------------------------------------------------------------------------
ISTcpServerCarat::~ISTcpServerCarat()
{
	if (HModuleCrypter) //Если модуль криптера загружен - выгружаем его из памяти
	{
		FreeLibrary(HModuleCrypter);
	}
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::Run(quint16 Port)
{
	if (!IsModeTest()) //Если тестовый режим - настройки не читаем
	{
		DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
		DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
		DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	}

	//Запуск локального сервера для контроля воркеров
	ServerController = new QLocalServer(this);
	ServerController->setMaxPendingConnections(1);
	if (!ServerController->listen(CARAT_CONTROLLER_PORT)) //Ошибка при запуске локального сервера
	{
		SetErrorString(ServerController->errorString());
		return false;
	}
	
	if (!ISTcpServerBase::Run(Port)) //Ошибка запуска основного сервера
	{
		return false;
	}

	if (!InitCrypter()) //Ошибка инициализации криптера
	{
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::SetDBHost(const QString &db_host)
{
	DBHost = db_host;
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::SetDBPort(int db_port)
{
	DBPort = db_port;
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::SetDBName(const QString &db_name)
{
	DBName = db_name;
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::incomingConnection(qintptr SocketDescriptor)
{
	ISTRACE();
	IsDisconnected = false;
	ISTcpServerBase::incomingConnection(SocketDescriptor);
	QTcpSocket *TcpSocket = nextPendingConnection();
	if (TcpSocket)
	{
		ISLOGGER_I(QString("Incoming auth from ") + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
		connect(TcpSocket, &QTcpSocket::disconnected, this, &ISTcpServerCarat::Disconnected);
	}
	else
	{
		ISLOGGER_E("nextPendingConnection return null QTcpSocket");
		return;
	}

	QByteArray Buffer;
	int Size = 0;

	if (!WaitToken(TcpSocket, Buffer)) //Ошибка ожидания токена
	{
		return;
	}
	
	while (true) //Ждём пока не придёт запрос
	{
		ISSleep(50);
		ISSystem::ProcessEvents();
		if (IsDisconnected) //Если сокет отключился - выходим из функции
		{
			return;
		}

		if (TcpSocket->bytesAvailable() > 0) //Если есть данные, которые можно прочитать
		{
			Buffer.append(TcpSocket->readAll()); //Читаем данные
			if (!Size) //Размеры ещё не известны - вытаскиваем их
			{
				Size = ISTcp::GetQuerySizeFromBuffer(Buffer);
				if (!Size) //Если размер не удалось вытащить - вероятно пришли невалидные данные - отключаем клиента
				{
					ISLOGGER_E("Not getting query size. Disconnecting client " + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
					TcpSocket->abort();
					return;
				}
			}
			if (Buffer.size() == Size) //Запрос пришёл полностью - выходим из цикла
			{
				break;
			}
		}
	}

	//Дешифруем запрос
	std::vector<unsigned char> Vector;
	ISAes256::decrypt(GetToken(), std::vector<unsigned char>(Buffer.begin(), Buffer.end()), Vector);

	//Проверка валидности запроса
	QVariantMap VariantMap;
	QString ErrorString;
	if (!ISTcp::IsValidQuery(QString::fromStdString(std::string(Vector.begin(), Vector.end())).toUtf8(), VariantMap, ErrorString)) //Ошибка парсинга
	{
		SendError(TcpSocket, "Error parse query: " + ErrorString);
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

	if (!VariantMap.contains("Login")) //Если поле с логином отсутствует
	{
		SendError(TcpSocket, "Not found field \"Login\"");
		return;
	}

	QString Login = VariantMap["Login"].toString();
	if (Login.isEmpty()) //Если поле с логином пустое
	{
		SendError(TcpSocket, "Field \"Login\" is empty");
		return;
	}

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
	if (!qSelectAuth.ExecuteFirst())
	{
		SendError(TcpSocket, "Unknown error checking user login");
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
		if (qSelectAuth.ReadColumn("usrs_group").toInt() == 0) //Привязка отсутствует
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
	if (ISDatabase::Instance().Connect(CONNECTION_USER, DBHost, DBPort, DBName, Login, Password))
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
	if (!StartWorker(TcpSocket, StringPort, Login, Password)) //Не удалось запустить воркер
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
void ISTcpServerCarat::Disconnected()
{
	sender()->deleteLater();
	IsDisconnected = true;
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::StartWorker(QTcpSocket *TcpSocket, const QString &Port, const QString &Login, const QString &Password)
{
	bool Result = QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_DIR + "/CaratWorker" + EXTENSION_BINARY,
		QStringList() << Port << Login << Password << GetTokenString(),
		ISDefines::Core::PATH_APPLICATION_DIR);
	if (Result)
	{
		Result = ServerController->waitForNewConnection(CARAT_TIMEOUT_STARTED_WORKER); //Ожидаем подтверждение запуска от воркера
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::WaitToken(QTcpSocket *TcpSocket, QByteArray &Buffer)
{
	bool Result = true;
	while (Result) //Ожидаем приход токена
	{
		ISSleep(50);
		ISSystem::ProcessEvents();
		if (IsDisconnected) //Если сокет отключился - выходим из функции
		{
			break;
		}

		if (TcpSocket->bytesAvailable() > 0) //Пришли данные - добавляем их в буфер
		{
			Buffer.append(TcpSocket->readAll());
			if (!Buffer.contains("IEND")) //Если изображение с токеном пришло не полностью - продолжаем ждать
			{
				continue;
			}

			//Изображение с токеном пришло - записываем его в файл и пытаемся расшифровать
			QString PathTempToken = ISDefines::Core::PATH_TEMP_DIR + "/TempToken";
			QFile FileToken(PathTempToken);
			Result = FileToken.open(QIODevice::WriteOnly);
			if (Result) //Файл успешно открыт - записываем в него данные
			{
				Result = FileToken.write(Buffer) == Buffer.size();
				FileToken.close();
				if (Result) //Запись файла прошла успешно - дешифруем его криптером
				{
					const char *Message = decrypt_message(PathTempToken.toStdString().c_str());
					if (Message) //Криптер вернул сообщение - проверяем его размер
					{
						size_t MessageSize = strlen(Message);
						if (MessageSize == CARAT_TOKEN_SIZE) //Размер правильный - сохраняем токен
						{
							SetToken(Message);
						}
						else //Размер токена неправильный
						{
							ISLOGGER_E(QString("Token size invalid. Size: %1. Done size: %2").arg(MessageSize).arg(CARAT_TOKEN_SIZE));
						}
					}
					else //Криптер ничего не вернул
					{
						ISLOGGER_E(QString("Error decrypt image token. File: %1. Error: %2").arg(PathTempToken).arg(get_error()));
					}
				}
				else //Размер записанных данных не соответствует размеру буфера - ошибка
				{
					ISLOGGER_E("Writed size not equal buffer size");
				}
			}
			else //Ошибка открытия файла
			{
				ISLOGGER_E(QString("Error write temp token. File: %1. Error: %2").arg(PathTempToken).arg(FileToken.errorString()));
			}

			if (!Result) //Где-то выше произошла ошибка - отключаем клиента
			{
				TcpSocket->abort();
			}
			break;
		}
	}
	Buffer.clear();
	return Result;
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::InitCrypter()
{
	HModuleCrypter = LoadLibrary(ISDefines::Core::PATH_LIB_CRYPTER.toStdString().c_str());
	if (HModuleCrypter == NULL) //Ошибка загрузки библиотеки
	{
		SetErrorString("Error loading crypt module");
		return false;
	}
	
	//Получение экземпляров функций
	decrypt_message = (DecryptMessage)GetProcAddress(HModuleCrypter, "DecryptMessage");
	get_error = (GetError)GetProcAddress(HModuleCrypter, "GetError");
	if (!decrypt_message || !get_error) //Функции библиотеки определены неправильно
	{
		SetErrorString("Error function address with name");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
