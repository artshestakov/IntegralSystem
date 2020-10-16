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
ISTcpSocket::ISTcpSocket(qintptr SocketDescriptor, QObject *parent) : QTcpSocket(parent)
{
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
		ISSleep(1);
		ISSystem::ProcessEvents();
		if (!Size) //������� ��� �� �������� - ����������� ��
		{
			Size = ISTcp::GetQuerySizeFromBuffer(Buffer);
			if (!Size) //���� ������ �� ������� �������� - �������� ������ ���������� ������ - ���������� ������
			{
				SendErrorQuery(LANG("Carat.Error.MessageSize"));
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
		SendErrorQuery(LANG("Carat.Error.ParseMessage").arg(ErrorString));
		return;
	}

	//���� ���� "Type" ��� - ������
	if (!VariantMap.contains("Type"))
	{
		SendErrorQuery(LANG("Carat.Error.InvalidMessage").arg("not found field \"Type\""));
		return;
	}

	//�������� �������� ���� "Type"
	QString TypeName = VariantMap["Type"].toString();

	//���� ���� "Type" ������ - ������
	if (TypeName.isEmpty())
	{
		SendErrorQuery(LANG("Carat.Error.InvalidMessage").arg("field \"Type\" is empty"));
		return;
	}

	//�������� ��� ��������� �� ��� ����� � ���� ��� ����������� - ������
	ISNamespace::ApiMessageType MessageType = GetMessageType(TypeName);
	if (MessageType == ISNamespace::AMT_Unknown)
	{
		SendErrorQuery(LANG("Carat.Error.InvalidMessageType").arg(TypeName));
		return;
	}

	Buffer.clear(); //������� �����

	//������ � ��������� ��������� � �������
	ISTcpQueue::Instance().AddMessage(new ISTcpMessage
	{
		MessageType,
		VariantMap["Parameters"].toMap(),
		this
	});
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SendErrorQuery(const QString &ErrorString)
{
	Buffer.clear(); //������� �����

	//������ ��������� ��������� � ������� ��� � ������� ����� ����������� ��������
	ISTcpQueue::Instance().AddMessage(new ISTcpMessage
	{
		this,
		ErrorString
	});
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
