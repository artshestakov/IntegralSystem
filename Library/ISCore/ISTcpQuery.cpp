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
	//Формируем запрос
	std::string String = ISSystem::VariantMapToJsonString(
	{
		{ "Type", QueryType },
		{ "Parameters", Parameters }
	}).simplified().toStdString();

	const std::vector<unsigned char> PlainVector(String.begin(), String.end());
	std::vector<unsigned char> EncryptedVector;

	//Шифруем
	size_t EncryptedSize = ISAes256::encrypt(ISTcpConnector::Instance().GetToken(), PlainVector, EncryptedVector);

	//Формируем вектор с размером шифрованного пакета и вставляем его в итоговый вектор
	std::string TempString = std::to_string(EncryptedSize);
	std::vector<char> TempVector = std::vector<char>(TempString.begin(), TempString.end());
	EncryptedVector.insert(EncryptedVector.begin(), SYMBOL_POINT);
	EncryptedVector.insert(EncryptedVector.begin(), TempVector.begin(), TempVector.end());

	//Получаем сокет и отправляем запрос
	QTcpSocket *TcpSocket = ISTcpConnector::Instance().GetSocket();
	if (TcpSocket->write((char *)EncryptedVector.data(), EncryptedVector.size()) != EncryptedSize)
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
		TcpSocket->waitForReadyRead();
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
