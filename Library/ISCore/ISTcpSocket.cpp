#include "ISTcpSocket.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
#include "ISLogger.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISLocalization.h"
#include "ISTcpQueue.h"
//-----------------------------------------------------------------------------
ISTcpSocket::ISTcpSocket(qintptr SocketDescriptor, QObject *parent)
	: QTcpSocket(parent),
	IsAuthorized(false)
{
	Functions[API_AUTH] = &ISTcpSocket::Auth;

	setSocketDescriptor(SocketDescriptor);
	connect(this, &ISTcpSocket::readyRead, this, &ISTcpSocket::ReadyRead);
}
//-----------------------------------------------------------------------------
ISTcpSocket::~ISTcpSocket()
{

}
//-----------------------------------------------------------------------------
void ISTcpSocket::ReadyRead()
{
	//��������� ��������� ������ ������ � �����
	Buffer.push_back(readAll());

	long Size = 0;
	while (true) //��� ���� �� ����� ������
	{
		ISSleep(10);
		ISSystem::ProcessEvents();
		if (!Size) //������� ��� �� �������� - ����������� ��
		{
			Size = ISTcp::GetQuerySizeFromBuffer(Buffer);
			if (!Size) //���� ������ �� ������� �������� - �������� ������ ���������� ������ - ���������� ������
			{
				SendError("Not getting query size");
				ClearBuffer();
				return;
			}
		}
		if (Buffer.size() == Size) //������ ������ ��������� - ������� �� �����
		{
			break;
		}
	}

	QString ErrorString;
	QVariantMap VariantMap = ISSystem::JsonStringToVariantMap(Buffer, &ErrorString);
	if (VariantMap.isEmpty() && !ErrorString.isEmpty()) //��� ����������� ��������� ������
	{
		SendError(LANG("Carat.Error.ParseMessage").arg(ErrorString));
		return;
	}

	//���� ���� "Type" ��� - ������
	if (!VariantMap.contains("Type"))
	{
		SendError(LANG("Carat.Error.InvalidMessage").arg("not found field \"Type\""));
		return;
	}

	//�������� �������� ���� "Type"
	QString MessageType = VariantMap["Type"].toString();

	//���� ���� "Type" ������ - ������
	if (MessageType.isEmpty())
	{
		SendError(LANG("Carat.Error.InvalidMessage").arg("field \"Type\" is empty"));
		return;
	}

	//������ � ��������� ��������� � �������
	ISTcpQueue::Instance().AddMessage(new ISTcpMessage
	{
		MessageType,
		VariantMap["Parameters"].toMap()
	});

	//���� ������ ������ �� ����������� � ������ ��� ����������� - ������
	//if (QueryType == API_AUTH && IsAuthorized)
	{
		//SendError(LANG("Carat.Error.AlreadyAuth"));
		//return;
	}

	//���� ������ �� ��������������� ������ � ������ ��� �� ����������� - ������
	//if (QueryType != API_AUTH && !IsAuthorized)
	{
		//SendError(LANG("Carat.Error.NotAuth"));
		//return;
	}

	//���� ������ ������ �� ����������� � ���� ������ ��� �� ����������� - ����������
	//if (QueryType == API_AUTH && !IsAuthorized)
	{

	}
}
//-----------------------------------------------------------------------------
void ISTcpSocket::ClearBuffer()
{
	Buffer.clear();
}
//-----------------------------------------------------------------------------
void ISTcpSocket::Send(const QVariantMap &Data)
{
	//���� ����� ��� ��� ��������� - ����������
	if (state() == QTcpSocket::ConnectedState)
	{
		//��������� �����
		QByteArray ByteArray = ISSystem::VariantMapToJsonString(Data, QJsonDocument::Compact).toUtf8();
		ByteArray.insert(0, QString("%1.").arg(ByteArray.size()));

		//���������� ������ � ��� ��������� ��� ��������
		write(ByteArray);
		ISTcp::WaitForBytesWritten(this);
	}
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SendError(const QString &error_string)
{
	//��������� ����� � ������� � ���������� ���
	Send(ISTcpAnswer(error_string));
	ISLOGGER_E(error_string);
}
//-----------------------------------------------------------------------------
void ISTcpSocket::Auth(const QVariantMap &VariantMap)
{

}
//-----------------------------------------------------------------------------
