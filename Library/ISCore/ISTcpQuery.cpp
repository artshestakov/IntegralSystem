#include "ISTcpQuery.h"
#include "ISTcpConnector.h"
#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAes256.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpQuery::ISTcpQuery(const QString &query_type)
	: ErrorString(NO_ERROR_STRING),
	QueryType(query_type),
	DecryptAnswer(true)
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
void ISTcpQuery::SetDecryptAnswer(bool decrypt_answer)
{
	DecryptAnswer = decrypt_answer;
}
//-----------------------------------------------------------------------------
void ISTcpQuery::BindValue(const QString &ParamterName, const QVariant &ParameterValue)
{
	Parameters.insert(ParamterName, ParameterValue);
}
//-----------------------------------------------------------------------------
bool ISTcpQuery::Execute()
{
	//Шифруем запрос
	QByteArray Encripted = ISTcp::Crypt(ISTcpConnector::Instance().GetToken(),
	{
		{ "Type", QueryType },
		{ "Parameters", Parameters }
	});

	//Получаем сокет и отправляем на него запрос
	QTcpSocket *TcpSocket = ISTcpConnector::Instance().GetSocket();
	if (TcpSocket->write(Encripted) != Encripted.size())
	{
		ErrorString = TcpSocket->errorString();
		return false;
	}

	//Принудительно отправляем
	if (!TcpSocket->flush())
	{
		ErrorString = "Error flush: " + TcpSocket->errorString();
		return false;
	}

	QByteArray ByteArray;
	long Size = 0;

	while (true) //Ждём пока не придёт ответ
	{
		ISSleep(50);
		ISSystem::ProcessEvents();
		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (!Size) //Размер ещё не известен - вытаскиваем его
			{
				Size = ISTcp::GetQuerySizeFromBuffer(ByteArray);
			}

			if (ByteArray.size() == Size) //Запрос пришёл полностью - выходим из цикла
			{
				break;
			}
		}
	}

	if (DecryptAnswer)
	{
		ByteArray = ISTcp::Decrypt(ISTcpConnector::Instance().GetToken(), ByteArray);
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
	TcpAnswer.remove("ErrorDescription");
	return true;
}
//-----------------------------------------------------------------------------
QVariantMap& ISTcpQuery::GetAnswer()
{
	return TcpAnswer;
}
//-----------------------------------------------------------------------------
