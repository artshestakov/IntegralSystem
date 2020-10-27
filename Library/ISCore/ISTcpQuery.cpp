#include "ISTcpQuery.h"
#include "ISTcpConnector.h"
#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISVersionInfo.h"
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
	//Формируем запрос (тип запроса, его параметры и системные поля)
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

	//Получаем сокет и отправляем на него запрос
	QTcpSocket *TcpSocket = ISTcpConnector::Instance().GetSocket();
	if (TcpSocket->write(ByteArray) != ByteArray.size())
	{
		ErrorString = TcpSocket->errorString();
		return false;
	}
	ISTcp::WaitForBytesWritten(TcpSocket); //Ждём пока данные уйдут

	ByteArray.clear();
	int Size = 0;
	
	while (true) //Ждём пока не придёт ответ
	{
		ISSleep(50);
		ISSystem::ProcessEvents();
		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (!Size) //Размер ещё не известен - вытаскиваем его
			{
				//Size = ISTcp::GetQuerySizeFromBuffer(ByteArray);
			}

			if (ByteArray.size() == Size) //Запрос пришёл полностью - выходим из цикла
			{
				break;
			}
		}
	}

	//Проверяем валидность ответа
	if (!ISTcp::IsValidAnswer(ByteArray, TcpAnswer, ErrorString))
	{
		return false;
	}

	//Проверяем запрос на ошибку
	if (TcpAnswer["IsError"].toBool())
	{
		ErrorString = TcpAnswer["ErrorDescription"].toString();
		return false;
	}

	TcpAnswer.remove("IsError");
	return true;
}
//-----------------------------------------------------------------------------
QVariantMap& ISTcpQuery::GetAnswer()
{
	return TcpAnswer;
}
//-----------------------------------------------------------------------------
