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
	//������ ���������� ������� ��� �������� ��������
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

	while (true) //��� ���� �� ����� ������
	{
		//���� ������ ��� � �� ������ - ��������� �������
		if (!TcpSocket->waitForReadyRead(CARAT_TIMEOUT_INCOMING_QUERY))
		{
			SendError(TcpSocket, "Query not received");
			return;
		}

		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (!Size) //������� ��� �� ��������� - ����������� ��
			{
				Size = ISTcp::GetQuerySizeFromBuffer(ByteArray);
			}

			if (!Size) //���� ������ �� ������� �������� - �������� ������ ���������� ������ - ��������� �������
			{
				SendError(TcpSocket, "Query is not a valid");
				return;
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
	if (!qSelectKeys.Execute()) //������ �������
	{
		SendError(TcpSocket, qSelectKeys.GetErrorString());
		return;
	}

	QString Login;
	QVariantMap VariantMap;
	while (qSelectKeys.Next()) //���������� �����
	{
		//�������� ��������� ����
		std::string Key = qSelectKeys.ReadColumn("Keys").toString().toStdString();
		std::vector<unsigned char> VectorKey(Key.begin(), Key.end());

		std::vector<unsigned char> Vector;
		ISAes256::decrypt(VectorKey, std::vector<unsigned char>(ByteArray.begin(), ByteArray.end()), Vector);

		//�������� ���������� �������
		QString ErrorString;
		if (ISTcp::IsValidQuery(QString::fromStdString(std::string(Vector.begin(), Vector.end())).toUtf8(), VariantMap, ErrorString)) //��������� ������ �������
		{
			Login = qSelectKeys.ReadColumn("usename").toString();
			break;
		}
	}

	if (Login.isEmpty()) //���� �� ������� ��������� ����
	{
		SendError(TcpSocket, "Failed to find decryption key");
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
	qSelectAuth.BindValue(":Password", Password);

	//���� ����� ����� � �� �� ����������
	if (!qSelectAuth.ExecuteFirst() && !qSelectAuth.GetCountResultRows())
	{
		SendError(TcpSocket, "Message.Error.LoginNotExist");
		return;
	}

	//���� ������ ������������
	if (!qSelectAuth.ReadColumn("password").toBool())
	{
		SendError(TcpSocket, "Message.Error.InvalidPassword");
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
	if (ISDatabase::Instance().Connect(CONNECTION_USER, CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE), Login, Password))
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
	bool Result = QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_DIR + "/CaratWorker" + EXTENSION_BINARY, QStringList() << StringPort << Login << Password, ISDefines::Core::PATH_APPLICATION_DIR);
	if (Result)
	{
		Result = ServerController->waitForNewConnection(CARAT_TIMEOUT_STARTED_WORKER); //������� ������������� ������� �� �������
	}
	
	if (!Result) //�� ������� ��������� ������
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
