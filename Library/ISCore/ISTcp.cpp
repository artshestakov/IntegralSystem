#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
bool ISTcp::IsValidQuery(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString, QString &QueryType)
{
	//Конвертируем набор байт в QVariantMap
    VariantMap = ISSystem::JsonStringToVariantMap(ByteArray, &ErrorString);
	if (VariantMap.isEmpty() && !ErrorString.isEmpty()) //При конвертации произошла ошибка
	{
		return false;
	}

	//Если поля "Type" нет - ошибка
	if (!VariantMap.contains("Type"))
	{
		ErrorString = "Not found field \"Type\"";
		return false;
	}
	
	//Получаем значение поля "Type"
	QueryType = VariantMap["Type"].toString();

	//Если поле "Type" пустое - ошибка
	if (QueryType.isEmpty())
	{
		ErrorString = "Field \"Type\" is empty";
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcp::IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString)
{
    VariantMap = ISSystem::JsonStringToVariantMap(ByteArray, &ErrorString);
	if (VariantMap.isEmpty() && !ErrorString.isEmpty())
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
		if (!VariantMap.contains("ErrorDescription")) //Если поле с описанием ошибки отсутствует
		{
			ErrorString = "Not found field \"ErrorDescription\"";
			return false;
		}

		if (VariantMap["ErrorDescription"].toString().isEmpty()) //Если поле с описанием ошибки пустое
		{
			ErrorString = "Empty field \"ErrorDescription\"";
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
long ISTcp::GetQuerySizeFromBuffer(QByteArray &ByteArray)
{
	QString Digits;
	for (int i = 0;i < ByteArray.size(); ++i) //Обходим весь массив
	{
		if (ByteArray[i] == SYMBOL_POINT && i) //Нашли точку и её индекс не нулевой - вытаскиваем левую часть и удаляем её из массива
		{
			Digits = ByteArray.left(i);
			ByteArray.remove(0, i + 1);
			break;
		}
	}

	//Цифры нашлись и массив не пустой - конвертируем строку с цифрами в целое число
	if (!Digits.isEmpty() && !ByteArray.isEmpty())
	{
		bool Ok = true;
		long Result = Digits.toInt(&Ok);
		if (Ok)
		{
			return Result;
		}
	}
	return 0;
}
//-----------------------------------------------------------------------------
void ISTcp::WaitForBytesWritten(QTcpSocket *TcpSocket)
{
	while (TcpSocket->bytesToWrite() > 0)
	{
		ISSystem::ProcessEvents();
		ISSleep(1);
	}
}
//-----------------------------------------------------------------------------
