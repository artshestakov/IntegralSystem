#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAes256.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
bool ISTcp::IsValidQuery(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString)
{
    VariantMap = ISSystem::JsonStringToVariantMap(ByteArray, &ErrorString);
	if (VariantMap.isEmpty() && !ErrorString.isEmpty())
	{
		return false;
	}

	if (!VariantMap.contains("Type"))
	{
		ErrorString = "Not found field \"Type\"";
		return false;
	}

	if (!VariantMap.contains("Parameters"))
	{
		ErrorString = "Not found field \"Parameters\"";
		return false;
	}
	
	if (!VariantMap["Type"].isValid())
	{
		ErrorString = "Empty field \"Type\"";
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

	if (!VariantMap.contains("IsError"))
	{
		ErrorString = "Not found field \"IsError\"";
		return false;
	}

	if (!VariantMap.contains("ErrorDescription"))
	{
		ErrorString = "Not found field \"ErrorDescription\"";
		return false;
	}

	if (!VariantMap["IsError"].isValid())
	{
		ErrorString = "Empty field \"IsError\"";
		return false;
	}

	if (!VariantMap["ErrorDescription"].isValid())
	{
		ErrorString = "Empty field \"ErrorDescription\"";
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
int ISTcp::GetQuerySizeFromBuffer(QByteArray &ByteArray)
{
	QString Digits;
	for (int i = 0;i < ByteArray.size(); ++i) //������� ���� ������
	{
		if (ByteArray[i] == SYMBOL_POINT && i) //����� ����� � � ������ �� ������� - ����������� ����� ����� � ������� � �� �������
		{
			Digits = ByteArray.left(i);
			ByteArray.remove(0, i + 1);
			break;
		}
	}

	//����� ������� � ������ �� ������ - ������������ ������ � ������� � ����� �����
	if (!Digits.isEmpty() && !ByteArray.isEmpty())
	{
		bool Ok = true;
		int Result = Digits.toInt(&Ok);
		if (Ok)
		{
			return Result;
		}
	}
	return 0;
}
//-----------------------------------------------------------------------------
QByteArray ISTcp::Crypt(const std::vector<unsigned char> &Key, const QVariantMap &VariantMap)
{
	//��������� ������ ��� ����������
	std::string String = ISSystem::VariantMapToJsonString(VariantMap).simplified().toStdString();
	const std::vector<unsigned char> VectorPlain(String.begin(), String.end());

	//�������
	std::vector<unsigned char> VectorEncrypted;
	size_t EncryptedSize = ISAes256::encrypt(Key, VectorPlain, VectorEncrypted);

	//��������� ������ ������������� ������ � ��������� ��� � �������� ������
	String = std::to_string(EncryptedSize);
	std::vector<char> TempVector = std::vector<char>(String.begin(), String.end());
	VectorEncrypted.insert(VectorEncrypted.begin(), SYMBOL_POINT); //����������� ������� � ������ - �����
	VectorEncrypted.insert(VectorEncrypted.begin(), TempVector.begin(), TempVector.end());
	EncryptedSize += (String.size() + 1);

	//���������
	QByteArray Encrypted(EncryptedSize, '\0');
	for (size_t i = 0; i < EncryptedSize; ++i)
	{
		Encrypted[i] = VectorEncrypted[i];
	}
	return Encrypted;
}
//-----------------------------------------------------------------------------
QByteArray ISTcp::Decrypt(const std::vector<unsigned char> &Key, const QByteArray &ByteArray)
{
	int Size = ByteArray.size();
	std::vector<unsigned char> Vector(Size);
	for (size_t i = 0; i < Size; ++i)
	{
		Vector[i] = ByteArray[i];
	}

	std::vector<unsigned char> Decrypted;
	size_t DecryptedSize = ISAes256::decrypt(Key, Vector, Decrypted);
	QByteArray Result(DecryptedSize, Qt::Uninitialized);
	for (size_t i = 0; i < DecryptedSize; ++i)
	{
		Result[i] = Decrypted[i];
	}
	return Result;
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
