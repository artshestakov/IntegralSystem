#include "ISConstantsOld.h"
#include "ISConstants.h"
#include "ISAlgorithmOld.h"
//-----------------------------------------------------------------------------
QString ISAlgorithmOld::GetLastErrorString()
{
	QString ErrorString;
#ifdef WIN32
    char Buffer[1024] = { 0 };
	DWORD Result = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), Buffer, 1024, NULL);
	if (Result != 0) //��������� �������� ������ ������ �������
	{
		ErrorString = QString::fromLocal8Bit(Buffer, (int)Result);
	}
	else //������
	{
		ErrorString = "FormatMessage API failed";
	}
#else
    char *Buffer = strerror(errno);
    ErrorString = QString::fromLocal8Bit(Buffer, strlen(Buffer));
#endif
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISAlgorithmOld::GenerateSalt(QString &Salt, QString &ErrorString)
{
	//��������� �������������� ������ � �����
	unsigned char Buffer[CARAT_SALT_SIZE] = { 0 };
#ifdef WIN32 //������������ ���� ��� Windows
	HCRYPTPROV CryptoProvider = 0;
	bool Result = CryptAcquireContext(&CryptoProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT) == TRUE;
	if (Result) //�������� ������ ������� - ��������� ����
	{
		Result = CryptGenRandom(CryptoProvider, CARAT_SALT_SIZE, Buffer) == TRUE;
		if (Result) //��������� ������ ������� - ����������� ��������
		{
			CryptReleaseContext(CryptoProvider, 0);
		}
		else //������ ���������
		{
			ErrorString = ISAlgorithmOld::GetLastErrorString();
		}
	}
	else //�� ������� ������� ��������
	{
		ErrorString = ISAlgorithmOld::GetLastErrorString();
	}
#else //������������ ���� ��� Linux
	FILE *FileDevice = fopen("/dev/random", "r");
	bool Result = FileDevice ? true : false;
	if (Result) //���������� ������� ������� - ������ � ��������� ����������
	{
		Result = fread(&Buffer[0], sizeof(char), CARAT_SALT_SIZE, FileDevice) == CARAT_SALT_SIZE;
		fclose(FileDevice);
	}
	else
	{
		ErrorString = ISAlgorithmOld::GetLastErrorString();
	}
#endif
	if (Result) //���� ��� ������ - ��������� ���� � HEX
	{
		Salt.clear();
		for (unsigned long i = 0; i < CARAT_SALT_SIZE; ++i) //������� ����� � �����
		{
			Salt.append(QByteArray(1, Buffer[i]).toHex().toUpper());
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
QString ISAlgorithmOld::SaltPassword(const QString &HashPassword, const QString &Salt)
{
	QString HashResult;
	for (int i = 0; i < (int)CARAT_HASH_SIZE; ++i)
	{
		HashResult.push_back(HashPassword[i]);
		HashResult.push_back(Salt[i]);
	}
	std::reverse(HashResult.begin(), HashResult.end());
	return HashResult;
}
//-----------------------------------------------------------------------------
bool ISAlgorithmOld::PasswordVerification(const QString &Password)
{
	//��������� ������ ������
	if (Password.size() < MINIMUM_PASSWORD_LENGHT)
	{
		return false;
	}

	//���� �����
	bool FoundDigit = false;
	for (const QChar &Char : Password)
	{
		if (Char.isDigit()) //�����
		{
			FoundDigit = true;
			break;
		}
	}
	if (!FoundDigit) //�� �����
	{
		return false;
	}

	//���� ����� � ������� ��������
	bool FoundUpper = false;
	for (const QChar &Char : Password)
	{
		if (Char.isUpper()) //�����
		{
			FoundUpper = true;
			break;
		}
	}
	if (!FoundUpper) //�� �����
	{
		return false;
	}

	//���� ����� � ������ ��������
	bool FoundLower = false;
	for (const QChar &Char : Password)
	{
		if (Char.isLower()) //�����
		{
			FoundLower = true;
			break;
		}
	}
	if (!FoundLower) //�� �����
	{
		return false;
	}

	//���� ����. �������
	bool FoundSpecial = false;
	for (const QChar &Symbol : QString(SYMBOL_SPECIAL_ARRAY))
	{
		if (Password.contains(Symbol))
		{
			FoundSpecial = true;
			break;
		}
	}
	if (!FoundSpecial)
	{
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISAlgorithmOld::FormatPhoneNumber(QString &PhoneNumber)
{
	for (int i = 0; i < PhoneNumber.size(); ++i) //������� ������ � ������� ��������
	{
		if (!PhoneNumber[i].isDigit()) //���� ������� ������ �� �������� ������
		{
			PhoneNumber.remove(i, 1); //������� ���� ������
			--i; //� ��������� ������
		}
	}
}
//-----------------------------------------------------------------------------
QString ISAlgorithmOld::FormatNumber(long long Number, char Separator)
{
	QString Result = QString::number(Number); //��������� ����� � ������

	//���� ����� ������ � �������� [-999;999] - ������� ������
	if ((Number > 0 && Number > 999) || (Number < 0 && Number < -999))
	{	
		int ResultSize = Result.size(); //�������� ������
		for (int i = ResultSize - 1, j = 1; i >= 0; --i, ++j)
		{
			if (j == 3) //���� �� ������ ���� ������ (� ��������� ���) - ��������� ������
			{
				Result.insert(i, Separator);
				++ResultSize;
				j = 0; //���������� ������� ��������
			}
		}

		if (Result[0] == Separator) //������� ������ ������ (���� ����)
		{
			Result.remove(0, 1);
		}
	}

	//����� �� ����� � �������� - ���������� ��� ��� ����
	return Result;
}
//-----------------------------------------------------------------------------
QString ISAlgorithmOld::FormatNumber(double Number, char Separator, unsigned int Precision)
{
    QString Result = DOUBLE_PREPAREN(Number, Precision); //��������� ����� � ������
	int PosPoint = Result.indexOf('.');
	if (PosPoint != -1)
	{
		bool Ok = true;
		long long Left = Result.mid(0, PosPoint).toLongLong(&Ok);
		if (Ok)
		{
            Result.replace(0, PosPoint, FormatNumber(Left, Separator));
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithmOld::GenerateUuidStandart()
{
	std::string StringUID;
#ifdef WIN32
	GUID UID = { 0 };
	HRESULT Result = CoCreateGuid(&UID); //���������� �������������
	if (Result == S_OK) //��������� ������ �������
	{
		unsigned char *Char = { 0 };
		if (UuidToString(&UID, &Char) == RPC_S_OK) //��������������� � ������
		{
            //��������� ������ � ��������� �
            StringUID.resize(UUID_STANDART_SIZE);
			for (size_t i = 0; i < UUID_STANDART_SIZE; ++i)
			{
				StringUID[i] = Char[i];
			}
		}
	}
#else
    //���������� �������������
    uuid_t UUID = { 0 };
    uuid_generate(UUID);

    //��������� ��� � ������
    char Char[UUID_STANDART_SIZE] = { 0 };
    uuid_unparse(UUID, Char);
    StringUID = Char;
#endif
	return StringUID;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithmOld::GenerateUuid()
{
	std::string UID = ISAlgorithmOld::GenerateUuidStandart();
	std::transform(UID.begin(), UID.end(), UID.begin(), toupper);
	return '{' + UID + '}';
}
//-----------------------------------------------------------------------------
std::string ISAlgorithmOld::GenerateUuidLite()
{
	std::string UID = ISAlgorithmOld::GenerateUuidStandart();
	std::transform(UID.begin(), UID.end(), UID.begin(), tolower);
	auto Begin = UID.begin();
	for (size_t i = UID.size() - 1; i > 0; --i)
	{
		if (UID[i] == '-')
		{
			UID.erase(Begin + i);
		}
	}
	return UID;
}
//-----------------------------------------------------------------------------
