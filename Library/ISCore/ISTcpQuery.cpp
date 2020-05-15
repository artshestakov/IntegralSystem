#include "ISTcpQuery.h"
#include "ISTcpConnector.h"
#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAes256.h"
//-----------------------------------------------------------------------------
ISTcpQuery::ISTcpQuery(const QString &query_type)
	: ErrorString(NO_ERROR_STRING),
	QueryType(query_type)
{
	
}
//-----------------------------------------------------------------------------
ISTcpQuery::~ISTcpQuery()
{

}
//-----------------------------------------------------------------------------
QString ISTcpQuery::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISTcpQuery::BindValue(const QString &ParamterName, const QVariant &ParameterValue)
{
	Parameters.insert(ParamterName, ParameterValue);
}
//-----------------------------------------------------------------------------
bool ISTcpQuery::Execute()
{
	//������� ������
	QByteArray Encripted = ISTcp::Crypt(ISTcpConnector::Instance().GetToken(),
	{
		{ "Type", QueryType },
		{ "Parameters", Parameters }
	});

	//�������� ����� � ���������� �� ���� ������
	QTcpSocket *TcpSocket = ISTcpConnector::Instance().GetSocket();
	if (TcpSocket->write(Encripted) != Encripted.size())
	{
		ErrorString = TcpSocket->errorString();
		return false;
	}

	//������������� ����������
	if (!TcpSocket->flush())
	{
		ErrorString = "Error flush: " + TcpSocket->errorString();
		return false;
	}

	QByteArray ByteArray;
	long Size = 0;

	while (true) //��� ���� �� ����� �����
	{
		TcpSocket->waitForReadyRead();
		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (!Size) //������ ��� �� �������� - ����������� ���
			{
				Size = ISTcp::GetQuerySizeFromBuffer(ByteArray);
			}

			if (ByteArray.size() == Size) //������ ������ ��������� - ������� �� �����
			{
				break;
			}
		}
	}

	//��������� ���������� ������
	if (!ISTcp::IsValidAnswer(ByteArray, TcpAnswer, ErrorString))
	{
		return false;
	}

	//��������� ������ �� ������
	if (TcpAnswer["IsError"].toBool())
	{
		ErrorString = TcpAnswer["ErrorDescription"].toString();
		return false;
	}

	TcpAnswer.remove("IsError");
	TcpAnswer.remove("ErrorDescription");
	return true;
}
//-----------------------------------------------------------------------------
QVariantMap& ISTcpQuery::GetAnswer()
{
	return TcpAnswer;
}
//-----------------------------------------------------------------------------
