#include "ISTcpServerCarat.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISDefinesCore.h"
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
ISTcpServerCarat::ISTcpServerCarat(QObject *parent) : ISTcpServerBase(parent)
{
	
}
//-----------------------------------------------------------------------------
ISTcpServerCarat::~ISTcpServerCarat()
{
	
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::incomingConnection(qintptr SocketDescriptor)
{
	ISTcpServerBase::incomingConnection(SocketDescriptor);
	QTcpSocket *TcpSocket = nextPendingConnection();
	connect(TcpSocket, &QTcpSocket::disconnected, TcpSocket, &QTcpSocket::deleteLater);

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
			if (!Size) //������ ��� �� �������� - ����������� ���
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
	
	//�������� ���������� �������
	QVariantMap VariantMap;
	QString ErrorString;
	if (!ISTcp::IsValidQuery(ByteArray, VariantMap, ErrorString))
	{
		SendError(TcpSocket, ErrorString);
		return;
	}

	//�������� ���� �������
	QString QueryType = VariantMap["Type"].toString();
	if (QueryType != API_AUTH) //���� �� ����������� - ������
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

	if (!VariantMap.contains("Password")) //���� ���� � ������� �����������
	{
		SendError(TcpSocket, "Not found field \"Password\"");
		return;
	}

	QString Login = VariantMap["Login"].toString();
	QString Password = VariantMap["Password"].toString();

	if (Login.isEmpty()) //���� ���� � ������� ������
	{
		SendError(TcpSocket, "Field \"Login\" is empty");
		return;
	}

	if (Password.isEmpty()) //���� ���� � ������� ������
	{
		SendError(TcpSocket, "Field \"Password\" is empty");
		return;
	}

	//�������� ��������� ������ �������������
	ISQuery qSelectAuth(QS_AUTH);
	qSelectAuth.BindValue(":Login", Login);

	//���� ����� ����� � �� �� ����������
	if (!qSelectAuth.ExecuteFirst() && !qSelectAuth.GetCountResultRows())
	{
		SendError(TcpSocket, "Message.Error.LoginNotExist");
		return;
	}

	//���� ������ ������������
	if (qSelectAuth.ReadColumn("passwd").toString() != ISSystem::StringToMD5(Password + Login))
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

	//���� ��������� ����
	QTcpServer TcpServer;
	quint16 Port = serverPort() + 1;
	for (Port; Port < USHRT_MAX; ++Port)
	{
		if (TcpServer.listen(QHostAddress::AnyIPv4, Port)) //���� ������� ��������� ���� - ��������� ��� � ������� �� �����
		{
			TcpServer.close();
			break;
		}
	}

	ISTcpAnswer TcpAnswer;
	QString StringPort = QString::number(Port);
	bool Started = QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_DIR + "/CaratWorker.exe", QStringList() << StringPort, ISDefines::Core::PATH_APPLICATION_DIR);
	if (Started)
	{
		TcpAnswer["Port"] = StringPort;
	}
	else
	{
		TcpAnswer.SetError("Error started worker");
	}
	Send(TcpSocket, TcpAnswer);
}
//-----------------------------------------------------------------------------
