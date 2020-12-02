#include "ISTcpSocket.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
#include "ISLogger.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISLocalization.h"
#include "ISTcpQueue.h"
#include "ISTcpMessage.h"
//-----------------------------------------------------------------------------
ISTcpSocket::ISTcpSocket(qintptr socket_descriptor, QObject *parent)
	: QTcpSocket(parent),
	SocketDescriptor(socket_descriptor),
	MessageSize(0),
	ChunkCount(0),
	IsAuthorized(false),
	UserID(0),
	UserGroupID(0),
	UserIsSystem(false)
{
	setSocketDescriptor(SocketDescriptor);
	Address = peerAddress().toString(); //����� ����� �������� ������ ����� ��������� ����������� �������� ������

	Timer = new QTimer(this);
	Timer->setInterval(5000);
	Timer->setSingleShot(true);
	connect(Timer, &QTimer::timeout, this, &ISTcpSocket::Timeout);

	//��� ������� ����������� ������ ������������ � ����� ������������
	connect(this, static_cast<void(ISTcpSocket::*)(QAbstractSocket::SocketError)>(&ISTcpSocket::error), this, &ISTcpSocket::Error);
	connect(this, &ISTcpSocket::readyRead, this, &ISTcpSocket::ReadyRead);
}
//-----------------------------------------------------------------------------
ISTcpSocket::~ISTcpSocket()
{
	if (Timer->isActive()) //���� ������ ��� ������� - ������������� ���
	{
		Timer->stop();
	}
}
//-----------------------------------------------------------------------------
qintptr ISTcpSocket::GetSocketDescriptor() const
{
	return SocketDescriptor;
}
//-----------------------------------------------------------------------------
QString ISTcpSocket::GetAddress() const
{
	return Address;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SetAuthorized(bool authorized)
{
	IsAuthorized = authorized;
}
//-----------------------------------------------------------------------------
bool ISTcpSocket::GetAuthorized() const
{
	return IsAuthorized;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SetUserID(int user_id)
{
	UserID = user_id;
}
//-----------------------------------------------------------------------------
int ISTcpSocket::GetUserID() const
{
	return UserID;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SetUserGroupID(int user_group_id)
{
	UserGroupID = user_group_id;
}
//-----------------------------------------------------------------------------
int ISTcpSocket::GetUserGroupID() const
{
	return UserGroupID;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SetUserIsSystem(bool user_is_system)
{
	UserIsSystem = user_is_system;
}
//-----------------------------------------------------------------------------
bool ISTcpSocket::GetUserIsSystem() const
{
	return UserIsSystem;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::ReadyRead()
{
	//��������� ��������� ������ ������ � �����, ������������� ������ � ��������� ������
	Buffer.push_back(readAll());
	Timer->start(); 
	if (!MessageSize) //���� ������ ��������� ��� �� ��������
	{
		bool Ok = true;
		MessageSize = ISTcp::GetQuerySizeFromBuffer(Buffer, Ok);
		if (!Ok) //���� �� ������� �������� ������ ��������� - ������������� ��������� �������, �������� �� ������ � ������� �����
		{
			abort();
			ISLOGGER_E(__CLASS__, "Not get message size. Client will be disconnected. Invalid message:\n" + Buffer);
			return;
		}
	}

	//�������������� ���������� ������ � ���������, �� ������ �� ��������� ���������
	++ChunkCount;
	if (Buffer[Buffer.size() - 1] != SYMBOL_NULL_TERM)
	{
		return;
	}

	//��������� ������ ���������, ������ ������� ������
	//���� ������� ����������, ������ ���-�� �� ��� � ��������� ������� �������������
	if ((unsigned int)Buffer.size() != MessageSize)
	{
		ISLOGGER_E(__CLASS__, QString("Invalid size. Declared size %1, read size %2").arg(MessageSize).arg(Buffer.size()));
		abort();
		ClearBuffer();
	}

	//������������� ������
	Timer->stop();

	//������ ��������� �� ���������
	ISTcpMessage *TcpMessage = new ISTcpMessage(this);
	TcpMessage->Size = Buffer.size();
	TcpMessage->ChunkCount = ChunkCount;

	//�������� ����� � ������ ���������
	QJsonParseError JsonParseError;
	ISTimePoint TimePoint = ISAlgorithm::GetTick();
	TcpMessage->Parameters = ISSystem::JsonStringToVariantMap(Buffer, JsonParseError);
	TcpMessage->ParseMSec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);

	bool Result = !TcpMessage->Parameters.isEmpty() && JsonParseError.error == QJsonParseError::NoError;
	if (Result) //����������� ������ �������
	{
		Result = TcpMessage->Parameters.contains("Type");
		if (Result) //���� ���� "Type" ����
		{
			//�������� �������� ���� "Type"
			TcpMessage->TypeName = TcpMessage->Parameters["Type"].toString();
			Result = !TcpMessage->TypeName.isEmpty();
			if (Result) //���� ���� "Type" �� ������
			{
				//�������� ��� ��������� �� ��� ����� � ���� ��� ����������� - ������
				ISNamespace::ApiMessageType MessageType = ISTcp::GetMessageTypeByName(TcpMessage->TypeName);
				Result = MessageType != ISNamespace::AMT_Unknown;
				if (Result) //��������� ��������
				{
					TcpMessage->Type = MessageType;
					TcpMessage->Parameters = TcpMessage->Parameters["Parameters"].toMap();
				}
				else //��� ��������� �� ���������
				{
					TcpMessage->SetErrorString(LANG("Carat.Error.InvalidMessageType").arg(TcpMessage->TypeName));
				}
			}
			else //���� "Type" ������
			{
				TcpMessage->SetErrorString(LANG("Carat.Error.InvalidMessage").arg("field \"Type\" is empty"));
			}
		}
		else //���� "Type" �����������
		{
			TcpMessage->SetErrorString(LANG("Carat.Error.InvalidMessage").arg("not found field \"Type\""));
		}
	}
	else //������ ��� ����������� ���������
	{
		TcpMessage->SetErrorString(LANG("Carat.Error.ParseMessage").arg(JsonParseError.errorString()));
	}
	
	//������� ����� � ��������� ��������� � �������
	ClearBuffer();
	ISTcpQueue::Instance().AddMessage(TcpMessage);
}
//-----------------------------------------------------------------------------
void ISTcpSocket::Error(QAbstractSocket::SocketError socket_error)
{
	if (socket_error != QAbstractSocket::RemoteHostClosedError)
	{
		ISLOGGER_E(__CLASS__, errorString());
	}
}
//-----------------------------------------------------------------------------
void ISTcpSocket::ClearBuffer()
{
	Buffer.clear();
	MessageSize = 0;
	ChunkCount = 0;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::Timeout()
{
	ISLOGGER_E(__CLASS__, "Message come not complete from " + peerAddress().toString());
	abort();
}
//-----------------------------------------------------------------------------
