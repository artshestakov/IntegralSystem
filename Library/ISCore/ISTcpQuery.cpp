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
	int AnswerSize = 0;

	while (true) //Ждём пока не придёт ответ
	{
		ISSleep(1);
		ISSystem::ProcessEvents();
		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (!AnswerSize) //Размер ещё не известен - вытаскиваем его
			{
				bool Ok = true;
				AnswerSize = ISTcp::GetQuerySizeFromBuffer(ByteArray, Ok);
				if (!Ok) //Не удалось вытащить размер ответа
				{
					ErrorString = "Error getting answer size";
					return false;
				}
			}

			if (ByteArray.size() == AnswerSize) //Запрос пришёл полностью - выходим из цикла
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
		ErrorString = TcpAnswer["ErrorString"].toString();
		return false;
	}

	TcpAnswer = TcpAnswer["Parameters"].toMap();
	Parameters.clear(); //Очищаем список параметров запроса
	return true;
}
//-----------------------------------------------------------------------------
QVariantMap& ISTcpQuery::GetAnswer()
{
	return TcpAnswer;
}
//-----------------------------------------------------------------------------
