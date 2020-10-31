#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
bool ISTcp::IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString)
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
unsigned int ISTcp::GetQuerySizeFromBuffer(QByteArray &ByteArray, bool &Ok)
{
    int Pos = ByteArray.indexOf(SYMBOL_POINT);
	Ok = Pos != -1;
	if (Ok)
	{
		QByteArray ByteArraySize = ByteArray.mid(0, Pos);
		unsigned int Result = ByteArraySize.toUInt(&Ok);
		if (Ok)
		{
			ByteArray.remove(0, Pos + 1);
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
