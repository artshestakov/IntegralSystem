#include "ISTcpQuery.h"
#include "ISTcpConnector.h"
#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISVersionInfo.h"
#include "ISLocalization.h"
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
	Parameters[ParamterName] = ParameterValue;
}
//-----------------------------------------------------------------------------
bool ISTcpQuery::Execute(const QString &query_type)
{
	QueryType = query_type;
	return Execute();
}
//-----------------------------------------------------------------------------
bool ISTcpQuery::Execute()
{
	//��������� ������ (��� �������, ��� ��������� � ��������� ����)
	QByteArray ByteArray = ISSystem::VariantMapToJsonString(
	{
		{ "Type", QueryType },
		{ "Parameters", Parameters },
		{
			"System", QVariantMap
	{
		{ "Version", ISVersionInfo::Instance().ToString() }
	}
		}
	}, QJsonDocument::Compact);
	ByteArray.insert(0, QString("%1.").arg(ByteArray.size()));

	//��������� ������� ����������
	if (!ISTcpConnector::Instance().IsConnected())
	{
		ErrorString = LANG("NotConnectToServer");
		return false;
	}

	//�������� ����� � ���������� �� ���� ������
	QTcpSocket *TcpSocket = ISTcpConnector::Instance().GetSocket();
	if (TcpSocket->write(ByteArray) != ByteArray.size())
	{
		ErrorString = TcpSocket->errorString();
		return false;
	}
	ISTcp::WaitForBytesWritten(TcpSocket); //��� ���� ������ �����

	ByteArray.clear();
	int AnswerSize = 0;

	while (true) //��� ���� �� ����� �����
	{
		ISSleep(1);
		PROCESS_EVENTS();
		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (!AnswerSize) //������ ��� �� �������� - ����������� ���
			{
				bool Ok = true;
				AnswerSize = ISTcp::GetQuerySizeFromBuffer(ByteArray, Ok);
				if (!Ok) //�� ������� �������� ������ ������
				{
					ErrorString = "Error getting answer size";
					return false;
				}
			}

			if (ByteArray.size() == AnswerSize) //������ ������ ��������� - ������� �� �����
			{
				break;
			}
		}
	}

	//��������� ���������� ������
	bool Result = IsValidAnswer(ByteArray, TcpAnswer);
	if (Result) //����� �������
	{
		//��������� ������ �� ������
		Result = !TcpAnswer["IsError"].toBool();
		if (Result)
		{
			TcpAnswer = TcpAnswer["Parameters"].toMap();
		}
		else
		{
			ErrorString = TcpAnswer["ErrorString"].toString();
		}
	}
	else //����� ���������� - ������� ��������� ������ (����� ��� ���-�� ����)
	{
		TcpAnswer.clear();
	}
	Parameters.clear(); //������� ������ ���������� �������
	ByteArray.clear(); //������� ��������� �����
	return Result;
}
//-----------------------------------------------------------------------------
QVariantMap& ISTcpQuery::GetAnswer()
{
	return TcpAnswer;
}
//-----------------------------------------------------------------------------
bool ISTcpQuery::IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap)
{
	QJsonParseError JsonParseError;
	VariantMap = ISSystem::JsonStringToVariantMap(ByteArray, JsonParseError);
	if (VariantMap.isEmpty() && JsonParseError.error != QJsonParseError::NoError)
	{
		return false;
	}

	if (!VariantMap.contains("IsError")) //���� ���� � ������ ������ �����������
	{
		ErrorString = "Not found field \"IsError\"";
		return false;
	}

	if (!VariantMap["IsError"].isValid()) //���� ���� � ������ ������ ����������
	{
		ErrorString = "Empty field \"IsError\"";
		return false;
	}

	if (VariantMap["IsError"].toBool()) //���� ������ ������������� ����, ������ ����� ��������� ������� � ��������
	{
		if (!VariantMap.contains("ErrorString")) //���� ���� � ��������� ������ �����������
		{
			ErrorString = "Not found field \"ErrorString\"";
			return false;
		}

		if (VariantMap["ErrorString"].toString().isEmpty()) //���� ���� � ��������� ������ ������
		{
			ErrorString = "Empty field \"ErrorString\"";
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
