#include "ISTcpSocket.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
#include "ISLogger.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISLocalization.h"
#include "ISTcpQueue.h"
#include "ISCountingTime.h"
#include "ISTcpMessage.h"
//-----------------------------------------------------------------------------
ISTcpSocket::ISTcpSocket(qintptr SocketDescriptor, QObject *parent)
	: QTcpSocket(parent),
	MessageSize(0)
{
	setSocketDescriptor(SocketDescriptor);

	Timer = new QTimer(this);
	Timer->setInterval(5000);
	Timer->setSingleShot(true);
	connect(Timer, &QTimer::timeout, this, &ISTcpSocket::abort);

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
			ISLOGGER_E("Not get message size. Client will be disconnected. Invalid message:\n" + Buffer);
			ClearBuffer();
			return;
		}
	}

	//���������, �� ������ �� ��������� ��������� - ������� ���� ������ �� ���������
	if (Buffer.size() != MessageSize)
	{
		return;
	}

	//������������� ������
	Timer->stop();

	//������ ��������� �� ���������
	ISTcpMessage *TcpMessage = new ISTcpMessage(this);

	QString TypeName;
	QJsonParseError JsonParseError;
	QVariantMap VariantMap = ISSystem::JsonStringToVariantMap(Buffer, JsonParseError);
	bool Result = !VariantMap.isEmpty() && JsonParseError.error == QJsonParseError::NoError;
	if (Result) //����������� ������ �������
	{
		Result = VariantMap.contains("Type");
		if (Result) //���� ���� "Type" ����
		{
			//�������� �������� ���� "Type"
			TypeName = VariantMap["Type"].toString();
			Result = !TypeName.isEmpty();
			if (Result) //���� ���� "Type" �� ������
			{
				//�������� ��� ��������� �� ��� ����� � ���� ��� ����������� - ������
				ISNamespace::ApiMessageType MessageType = GetMessageType(TypeName);
				Result = MessageType != ISNamespace::AMT_Unknown;
				if (Result) //��������� ��������
				{
					TcpMessage->Type = MessageType;
					TcpMessage->Parameters = VariantMap["Parameters"].toMap();
				}
				else //��� ��������� �� ���������
				{
					TcpMessage->SetErrorString(LANG("Carat.Error.InvalidMessageType").arg(TypeName));
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
	Q_UNUSED(socket_error);
	ISLOGGER_E(errorString());
}
//-----------------------------------------------------------------------------
ISNamespace::ApiMessageType ISTcpSocket::GetMessageType(const QString &TypeName) const
{
	if (TypeName == API_AUTH)
	{
		return ISNamespace::AMT_Auth;
	}
	return ISNamespace::AMT_Unknown;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::ClearBuffer()
{
	Buffer.clear();
	MessageSize = 0;
}
//-----------------------------------------------------------------------------
