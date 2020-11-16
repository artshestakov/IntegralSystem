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

	//Проверяем наличие соединения
	if (!ISTcpConnector::Instance().IsConnected())
	{
		ErrorString = LANG("NotConnectToServer");
		return false;
	}

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
		PROCESS_EVENTS();
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
	bool Result = IsValidAnswer(ByteArray, TcpAnswer);
	if (Result) //Ответ валиден
	{
		//Проверяем запрос на ошибку
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
	else //Ответ невалидный - очищаем структуру ответа (вдруг там что-то есть)
	{
		TcpAnswer.clear();
	}
	Parameters.clear(); //Очищаем список параметров запроса
	ByteArray.clear(); //Очищаем временный буфер
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

	if (!VariantMap.contains("IsError")) //Если поле с флагом ошибки отсутствует
	{
		ErrorString = "Not found field \"IsError\"";
		return false;
	}

	if (!VariantMap["IsError"].isValid()) //Если поле с флагом ошибки невалидное
	{
		ErrorString = "Empty field \"IsError\"";
		return false;
	}

	if (VariantMap["IsError"].toBool()) //Если ошибка действительно есть, только тогда проверяем наличие её описания
	{
		if (!VariantMap.contains("ErrorString")) //Если поле с описанием ошибки отсутствует
		{
			ErrorString = "Not found field \"ErrorString\"";
			return false;
		}

		if (VariantMap["ErrorString"].toString().isEmpty()) //Если поле с описанием ошибки пустое
		{
			ErrorString = "Empty field \"ErrorString\"";
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
