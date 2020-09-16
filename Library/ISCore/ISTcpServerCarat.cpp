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
#include "ISTrace.h"
#include "ISAlgorithm.h"
#include "ISLocalization.h"
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
	: QTcpServer(parent),
	ErrorString(NO_ERROR_STRING),
	IsDisconnected(false)
{
	
}
//-----------------------------------------------------------------------------
ISTcpServerCarat::~ISTcpServerCarat()
{
	
}
//-----------------------------------------------------------------------------
QString ISTcpServerCarat::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISTcpServerCarat::Run(quint16 Port)
{
	DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	
	bool Result = listen(QHostAddress::AnyIPv4, Port);
	if (Result)
	{
		connect(this, &QTcpServer::acceptError, this, &ISTcpServerCarat::AcceptError);
	}
	else
	{
		ErrorString = errorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::incomingConnection(qintptr SocketDescriptor)
{
	ISTRACE();
	IsDisconnected = false;
	QTcpServer::incomingConnection(SocketDescriptor);
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
	long Size = 0;

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

	//�������� ���������� �������
	QVariantMap VariantMap;
	QString error_string;
	if (!ISTcp::IsValidQuery(Buffer, VariantMap, error_string)) //������ ��������
	{
		SendError(TcpSocket, LANG("CaratError.ParseQuery").arg(error_string));
		return;
	}

	//���� ���� � ����� ������� �����������
	if (!VariantMap.contains("Type"))
	{
		SendError(TcpSocket, LANG("CaratError.NotFoundField").arg("Type"));
		return;
	}

	//���� ���� � ����� ������� ������
	QString QueryType = VariantMap["Type"].toString();
	if (QueryType.isEmpty())
	{
		SendError(TcpSocket, LANG("CaratError.FieldIsEmpty").arg("Type"));
		return;
	}

	//�������� ���� �������, ���� �� ����������� - ������
	if (QueryType != API_AUTH)
	{
		SendError(TcpSocket, LANG("CaratError.InvalidQueryType").arg(QueryType));
		return;
	}

	VariantMap = VariantMap["Parameters"].toMap();
	if (!VariantMap.contains("Login")) //���� ���� � ������� �����������
	{
		SendError(TcpSocket, LANG("CaratError.NotFoundField").arg("Login"));
		return;
	}

	QString Login = VariantMap["Login"].toString();
	if (Login.isEmpty()) //���� ���� � ������� ������
	{
		SendError(TcpSocket, LANG("CaratError.FieldIsEmpty").arg("Login"));
		return;
	}

	if (!VariantMap.contains("Password")) //���� ���� � ������� �����������
	{
		SendError(TcpSocket, LANG("CaratError.NotFoundField").arg("Password"));
		return;
	}

	QString Password = VariantMap["Password"].toString();
	if (Password.isEmpty()) //���� ���� � ������� ������
	{
		SendError(TcpSocket, LANG("CaratError.FieldIsEmpty").arg("Password"));
		return;
	}

	//�������� ������������
	ISQuery qSelectAuth(QS_AUTH);
	qSelectAuth.BindValue(":Login", Login);
	if (!qSelectAuth.ExecuteFirst())
	{
		SendError(TcpSocket, LANG("CaratError.CheckingLogin"));
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
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::Disconnected()
{
	sender()->deleteLater();
	IsDisconnected = true;
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::AcceptError(QTcpSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::Send(QTcpSocket *TcpSocket, const QVariantMap &Data)
{
	//���� ����� ��� ��� ��������� - ����������
	if (TcpSocket->state() == QTcpSocket::ConnectedState)
	{
		//��������� �����
		QByteArray ByteArray = ISSystem::VariantMapToJsonString(Data, QJsonDocument::Compact).toUtf8();
		ByteArray.insert(0, QString("%1.").arg(ByteArray.size()));

		//���������� ������ � ��� ��������� ��� ��������
		TcpSocket->write(ByteArray);
		ISTcp::WaitForBytesWritten(TcpSocket);
	}
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::SendError(QTcpSocket *TcpSocket, const QString &error_string)
{
	//��������� ����� � �������
	ISTcpAnswer TcpAnswer;
	TcpAnswer.SetError(error_string);

	//���������� � �������� ����������
	Send(TcpSocket, TcpAnswer);
	TcpSocket->abort();
	ISLOGGER_E(error_string);
}
//-----------------------------------------------------------------------------
