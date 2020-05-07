#include "ISTcpQuery.h"
#include "ISTcpConnector.h"
#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
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
void ISTcpQuery::BindValue(const QString &ParamterName, const QString &ParameterValue)
{
	Parameters.insert(ParamterName, ParameterValue);
}
//-----------------------------------------------------------------------------
bool ISTcpQuery::Execute(bool Async)
{
	//Формируем запрос
	QString String = ISSystem::VariantMapToJsonString(
	{
		{ "Type", QueryType },
		{ "Parameters", Parameters }
	}).simplified() + CARAT_PACKET_SEPARATOR;
	
	QTcpSocket *TcpSocket = ISTcpConnector::Instance().GetSocket();
	if (TcpSocket->write(String.toUtf8()) != String.size())
	{
		ErrorString = TcpSocket->errorString();
		return false;
	}

	if (!TcpSocket->flush())
	{
		ErrorString = "Error flush: " + TcpSocket->errorString();
		return false;
	}

	QByteArray ByteArray; //Ответ
	while (true)
	{
		TcpSocket->waitForReadyRead();
		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (ByteArray.right(CARAT_PACKET_SEPARATOR_SIZE) == CARAT_PACKET_SEPARATOR)
			{
				ByteArray.remove(ByteArray.size() - CARAT_PACKET_SEPARATOR_SIZE, CARAT_PACKET_SEPARATOR_SIZE);
				break;
			}
		}
	}

	QVariantMap VariantMap;
	if (!ISTcp::IsValidAnswer(ByteArray, VariantMap, ErrorString))
	{
		return false;
	}

	if (VariantMap["IsError"].toBool())
	{
		ErrorString = VariantMap["ErrorDescription"].toString();
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
