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
static QString QS_KEYS = PREPARE_QUERY("SELECT md5(md5(usename || :Port) || right(passwd, length(passwd) - 3)) AS Keys "
									   "FROM pg_shadow "
									   "WHERE passwd IS NOT NULL "
									   "ORDER BY usename");
//-----------------------------------------------------------------------------
static QString QS_AUTH = PREPARE_QUERY("SELECT "
									   "usrs_issystem, "
									   "usrs_isdeleted, "
									   "(usrs_group != 0)::BOOLEAN AS usrs_group, "
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
	if (HModuleCrypter)
	{
		FreeLibrary(HModuleCrypter);
	}
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::Run(quint16 Port)
{
	if (!IsModeTest()) //���� �������� ����� - ��������� �� ������
	{
		DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
		DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
		DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	}

	//������ ���������� ������� ��� �������� ��������
	ServerController = new QLocalServer(this);
	ServerController->setMaxPendingConnections(1);
	if (!ServerController->listen(CARAT_CONTROLLER_PORT)) //������ ��� ������� ���������� �������
	{
		SetErrorString(ServerController->errorString());
		return false;
	}
	
	if (!ISTcpServerBase::Run(Port)) //������ ������� ��������� �������
	{
		return false;
	}

	if (!InitCrypter())
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
		connect(TcpSocket, &QTcpSocket::disconnected, this, &ISTcpServerCarat::DisconnectedClient);
	}
	else
	{
		ISLOGGER_E("nextPendingConnection return null QTcpSocket");
		return;
	}

	QByteArray ByteArray;
	int Size = 0;

	while (true) //��� ���� �� ����� �����
	{
		ISSleep(50);
		ISSystem::ProcessEvents();
		if (IsDisconnected) //���� ����� ���������� - ������� �� �������
		{
			return;
		}

		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (ByteArray.contains("IEND"))
			{
				QString PathTempToken = ISDefines::Core::PATH_TEMP_DIR + "/TempToken";
				QFile FileToken(PathTempToken);
				if (FileToken.open(QIODevice::WriteOnly))
				{
					FileToken.write(ByteArray);
					FileToken.close();
					const char *Token = decrypt_message(PathTempToken.toStdString().c_str());
					Token = NULL;
				}
				else
				{
					ISLOGGER_E("Error write temp token: " + FileToken.errorString());
					TcpSocket->abort();
					return;
				}
				break;
			}
		}
	}
	
	Size = 0; //�������� ���������� � ��������

	while (true) //��� ���� �� ����� ������
	{
		ISSleep(50);
		ISSystem::ProcessEvents();
		if (IsDisconnected) //���� ����� ���������� - ������� �� �������
		{
			return;
		}

		if (TcpSocket->bytesAvailable() > 0) //���� ���� ������, ������� ����� ���������
		{
			ByteArray.append(TcpSocket->readAll()); //������ ������
			if (!Size) //������� ��� �� ��������� - ����������� ��
			{
				Size = ISTcp::GetQuerySizeFromBuffer(ByteArray);
				if (!Size) //���� ������ �� ������� �������� - �������� ������ ���������� ������ - ��������� �������
				{
					ISLOGGER_E("Not getting query size. Disconnecting client " + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
					TcpSocket->abort();
					return;
				}
			}
			if (ByteArray.size() == Size) //������ ������ ��������� - ������� �� �����
			{
				break;
			}
		}
	}

	//����������� ��� �����
	ISQuery qSelectKeys(QS_KEYS);
	qSelectKeys.BindValue(":Port", TcpSocket->peerPort());
	if (!qSelectKeys.Execute()) //������ ������� - ��������� �������
	{
		ISLOGGER_E("Not getting keys: " + qSelectKeys.GetErrorString());
		TcpSocket->abort();
		return;
	}

	bool Decrypted = false;
	QVariantMap VariantMap;
	std::string Key;
	while (qSelectKeys.Next()) //���������� �����
	{
		//�������� ��������� ����
		Key = qSelectKeys.ReadColumn("Keys").toString().toStdString();
		std::vector<unsigned char> VectorKey(Key.begin(), Key.end());

		std::vector<unsigned char> Vector;
		ISAes256::decrypt(VectorKey, std::vector<unsigned char>(ByteArray.begin(), ByteArray.end()), Vector);

		//�������� ���������� �������
		QString ErrorString;
		Decrypted = ISTcp::IsValidQuery(QString::fromStdString(std::string(Vector.begin(), Vector.end())).toUtf8(), VariantMap, ErrorString);
		if (Decrypted) //��������� ������ �������
		{
			break;
		}
	}

	if (!Decrypted)
	{
		SendError(TcpSocket, "Invalid login or password");
		return;
	}

	//���� ���� � ����� ������� �����������
	if (!VariantMap.contains("Type"))
	{
		SendError(TcpSocket, "Not found field \"Type\"");
		return;
	}

	//���� ���� � ����� ������� ������
	QString QueryType = VariantMap["Type"].toString();
	if (QueryType.isEmpty())
	{
		SendError(TcpSocket, "Query type not specified");
		return;
	}

	//�������� ���� �������, ���� �� ����������� - ������
	if (QueryType != API_AUTH)
	{
		SendError(TcpSocket, QString("Invalid query type \"%1\"").arg(QueryType));
		return;
	}

	VariantMap = VariantMap["Parameters"].toMap();

	if (!VariantMap.contains("Login")) //���� ���� � ������� �����������
	{
		SendError(TcpSocket, "Not found field \"Login\"");
		return;
	}

	QString Login = VariantMap["Login"].toString();
	if (Login.isEmpty()) //���� ���� � ������� ������
	{
		SendError(TcpSocket, "Field \"Login\" is empty");
		return;
	}

	if (!VariantMap.contains("Password")) //���� ���� � ������� �����������
	{
		SendError(TcpSocket, "Not found field \"Password\"");
		return;
	}

	QString Password = VariantMap["Password"].toString();
	if (Password.isEmpty()) //���� ���� � ������� ������
	{
		SendError(TcpSocket, "Field \"Password\" is empty");
		return;
	}

	//�������� ������������
	ISQuery qSelectAuth(QS_AUTH);
	qSelectAuth.BindValue(":Login", Login);
	if (!qSelectAuth.ExecuteFirst())
	{
		SendError(TcpSocket, "Unknown error checking user login");
		return;
	}

	//���� ����� ����� ������� �� ��������
	if (qSelectAuth.ReadColumn("usrs_isdeleted").toBool())
	{
		SendError(TcpSocket, "Message.Error.CurrentUserIsDeleted");
		return;
	}

	//���� � ������������ ��� ����� �������
	if (!qSelectAuth.ReadColumn("usrs_accessallowed").toBool())
	{
		SendError(TcpSocket, "Message.Error.User.NotAccessAllowed");
		return;
	}

	//�������� ������� �������� ������������ � ������
	if (!qSelectAuth.ReadColumn("usrs_issystem").toBool()) //���� ������������ �� ��������� - ��������� ��������
	{
		if (!qSelectAuth.ReadColumn("usrs_group").toBool()) //�������� �����������
		{
			SendError(TcpSocket, "Message.Error.User.NotLinkWithGroup");
			return;
		}
	}

	//���� ��� ������������ ��������� ����������� ����� �������� ������� ������
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

	//�������� ���������� � �� �� ������ � ������
	if (ISDatabase::Instance().Connect(CONNECTION_USER, DBHost, DBPort, DBName, Login, Password))
	{
		ISDatabase::Instance().Disconnect(CONNECTION_USER);
	}
	else //���� ���������� � �� ��������� � �������
	{
		SendError(TcpSocket, ISDatabase::Instance().GetErrorString());
		return;
	}

	ISLOGGER_I("Auth is done");

	//���� ��������� ����
	QTcpServer TcpServer;
	quint16 Port = serverPort() + 1;
	for (; Port < USHRT_MAX; ++Port)
	{
		if (TcpServer.listen(QHostAddress::AnyIPv4, Port)) //���� ������� ��������� ���� - ��������� ��� � ������� �� �����
		{
			TcpServer.close();
			break;
		}
	}

	//������ �������
	QString StringPort = QString::number(Port);
	if (!StartWorker(TcpSocket, StringPort, Login, Password, QString::fromStdString(Key))) //�� ������� ��������� ������
	{
		SendError(TcpSocket, "Message.Error.StartedWorker");
		return;
	}

	//��������� ����� � ������ � ���������� ���
	ISTcpAnswer TcpAnswer;
	TcpAnswer["Port"] = StringPort;
	Send(TcpSocket, std::vector<unsigned char>(), TcpAnswer);
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::Disconnected()
{
	sender()->deleteLater();
	IsDisconnected = true;
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::StartWorker(QTcpSocket *TcpSocket, const QString &Port, const QString &Login, const QString &Password, const QString &Key)
{
	bool Result = QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_DIR + "/CaratWorker" + EXTENSION_BINARY,
		QStringList() << Port << Login << Password << Key,
		ISDefines::Core::PATH_APPLICATION_DIR);
	if (Result)
	{
		Result = ServerController->waitForNewConnection(CARAT_TIMEOUT_STARTED_WORKER); //������� ������������� ������� �� �������
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::InitCrypter()
{
	HModuleCrypter = LoadLibrary(ISDefines::Core::PATH_LIB_CRYPTER.toStdString().c_str());
	if (HModuleCrypter == NULL) //������ �������� ����������
	{
		SetErrorString("Error loading crypt module");
		return false;
	}
	
	//�������� ����������� �������
	decrypt_message = (DecryptMessage)GetProcAddress(HModuleCrypter, "DecryptMessage");
	get_error = (GetError)GetProcAddress(HModuleCrypter, "GetError");

	if (!decrypt_message || !get_error) //������� ���������� ���������� �����������
	{
		SetErrorString("Error function address with name");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
