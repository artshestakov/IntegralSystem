#include "ISTcpQuery.h"
#include "ISSystem.h"
#include "ISTcpConnector.h"
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
	bool Result = TcpSocket->write(String.toUtf8()) == String.size();
	if (Result)
	{
		Result = TcpSocket->flush();
		if (!Result)
		{
			ErrorString = "Error flush: " + TcpSocket->errorString();
		}
	}
	else //Ошибка отправки
	{
		ErrorString = TcpSocket->errorString();
	}

	if (Result)
	{

	}

	return Result;
}
//-----------------------------------------------------------------------------
