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
	if (HModuleCrypter) //���� ������ �������� �������� - ��������� ��� �� ������
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

	if (!InitCrypter()) //������ ������������� ��������
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

	if (!WaitToken(TcpSocket, Buffer)) //������ �������� ������
	{
		return;
	}
	
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
			Buffer.append(TcpSocket->readAll()); //������ ������
			if (!Size) //������� ��� �� �������� - ����������� ��
			{
				Size = ISTcp::GetQuerySizeFromBuffer(Buffer);
				if (!Size) //���� ������ �� ������� �������� - �������� ������ ���������� ������ - ��������� �������
				{
					ISLOGGER_E("Not getting query size. Disconnecting client " + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
					TcpSocket->abort();
					return;
				}
			}
			if (Buffer.size() == Size) //������ ������ ��������� - ������� �� �����
			{
				break;
			}
		}
	}

	//��������� ������
	std::vector<unsigned char> Vector;
	ISAes256::decrypt(GetToken(), std::vector<unsigned char>(Buffer.begin(), Buffer.end()), Vector);

	//�������� ���������� �������
	QVariantMap VariantMap;
	QString ErrorString;
	if (!ISTcp::IsValidQuery(QString::fromStdString(std::string(Vector.begin(), Vector.end())).toUtf8(), VariantMap, ErrorString)) //������ ��������
	{
		SendError(TcpSocket, "Error parse query: " + ErrorString);
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
		if (qSelectAuth.ReadColumn("usrs_group").toInt() == 0) //�������� �����������
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
	if (!StartWorker(TcpSocket, StringPort, Login, Password)) //�� ������� ��������� ������
	{
		SendError(TcpSocket, "Message.Error.StartedWorker");
		return;
	}

	//��������� ����� � ������ � ���������� ���
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
		Result = ServerController->waitForNewConnection(CARAT_TIMEOUT_STARTED_WORKER); //������� ������������� ������� �� �������
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::WaitToken(QTcpSocket *TcpSocket, QByteArray &Buffer)
{
	bool Result = true;
	while (Result) //������� ������ ������
	{
		ISSleep(50);
		ISSystem::ProcessEvents();
		if (IsDisconnected) //���� ����� ���������� - ������� �� �������
		{
			break;
		}

		if (TcpSocket->bytesAvailable() > 0) //������ ������ - ��������� �� � �����
		{
			Buffer.append(TcpSocket->readAll());
			if (!Buffer.contains("IEND")) //���� ����������� � ������� ������ �� ��������� - ���������� �����
			{
				continue;
			}

			//����������� � ������� ������ - ���������� ��� � ���� � �������� ������������
			QString PathTempToken = ISDefines::Core::PATH_TEMP_DIR + "/TempToken";
			QFile FileToken(PathTempToken);
			Result = FileToken.open(QIODevice::WriteOnly);
			if (Result) //���� ������� ������ - ���������� � ���� ������
			{
				Result = FileToken.write(Buffer) == Buffer.size();
				FileToken.close();
				if (Result) //������ ����� ������ ������� - ��������� ��� ���������
				{
					const char *Message = decrypt_message(PathTempToken.toStdString().c_str());
					if (Message) //������� ������ ��������� - ��������� ��� ������
					{
						size_t MessageSize = strlen(Message);
						if (MessageSize == CARAT_TOKEN_SIZE) //������ ���������� - ��������� �����
						{
							SetToken(Message);
						}
						else //������ ������ ������������
						{
							ISLOGGER_E(QString("Token size invalid. Size: %1. Done size: %2").arg(MessageSize).arg(CARAT_TOKEN_SIZE));
						}
					}
					else //������� ������ �� ������
					{
						ISLOGGER_E(QString("Error decrypt image token. File: %1. Error: %2").arg(PathTempToken).arg(get_error()));
					}
				}
				else //������ ���������� ������ �� ������������� ������� ������ - ������
				{
					ISLOGGER_E("Writed size not equal buffer size");
				}
			}
			else //������ �������� �����
			{
				ISLOGGER_E(QString("Error write temp token. File: %1. Error: %2").arg(PathTempToken).arg(FileToken.errorString()));
			}

			if (!Result) //���-�� ���� ��������� ������ - ��������� �������
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
	if (HModuleCrypter == NULL) //������ �������� ����������
	{
		SetErrorString("Error loading crypt module");
		return false;
	}
	
	//��������� ����������� �������
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
