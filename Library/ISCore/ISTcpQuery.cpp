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
	//��������� ������
	std::string String = ISSystem::VariantMapToJsonString(
	{
		{ "Type", QueryType },
		{ "Parameters", Parameters }
	}).simplified().toStdString();

	const std::vector<unsigned char> PlainVector(String.begin(), String.end());
	std::vector<unsigned char> EncryptedVector;

	//�������
	size_t EncryptedSize = ISAes256::encrypt(ISTcpConnector::Instance().GetToken(), PlainVector, EncryptedVector);

	//��������� ������ � �������� ������������ ������ � ��������� ��� � �������� ������
	std::string TempString = std::to_string(EncryptedSize);
	std::vector<char> TempVector = std::vector<char>(TempString.begin(), TempString.end());
	EncryptedVector.insert(EncryptedVector.begin(), SYMBOL_POINT);
	EncryptedVector.insert(EncryptedVector.begin(), TempVector.begin(), TempVector.end());

	//�������� ����� � ���������� ������
	QTcpSocket *TcpSocket = ISTcpConnector::Instance().GetSocket();
	if (TcpSocket->write((char *)EncryptedVector.data(), EncryptedVector.size()) != EncryptedSize)
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
