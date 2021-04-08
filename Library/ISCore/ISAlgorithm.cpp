#include "ISConstants.h"
#include "ISAlgorithm.h"
#include <openssl/sha.h>
#include <openssl/md5.h>
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetFileNameFromPath(const std::string &FilePath)
{
	size_t Pos = FilePath.rfind(PATH_SEPARATOR);
	if (Pos != std::string::npos)
	{
		return std::string(FilePath).erase(0, Pos + 1);
	}
	return FilePath;
}
//-----------------------------------------------------------------------------
QString ISAlgorithm::StringFromSize(qint64 FileSize)
{
    int Index = 0;
    for (; FileSize > 1023; FileSize /= 1024, ++Index) {}
    QString String = ISAlgorithm::FormatNumber(FileSize) + "BKMGT"[Index];
	if (String.contains("B"))
	{
		String.replace("B", " B");
	}
	else if (String.contains("K"))
	{
		String.replace("K", " Kb");
	}
	else if (String.contains("M"))
	{
		String.replace("M", " Mb");
	}
	else if (String.contains("G"))
	{
		String.replace("G", " Gb");
	}
	else if (String.contains("T"))
	{
		String.replace("T", " Tb");
	}
	return String;
}
//-----------------------------------------------------------------------------
quint64 ISAlgorithm::DirSize(const QString &PathDir, const QStringList &Filter)
{
	quint64 Size = 0;
	QDirIterator DirIterator(PathDir, Filter, QDir::Files, QDirIterator::Subdirectories);
	while (DirIterator.hasNext())
	{
		QFile File(DirIterator.next());
		Size += File.size();
	}
	return Size;
}
//-----------------------------------------------------------------------------
ISTimePoint ISAlgorithm::GetTick()
{
	return std::chrono::steady_clock::now();
}
//-----------------------------------------------------------------------------
unsigned long long ISAlgorithm::GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(T1 - T2).count();
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetClassName(const std::string &FunctionName)
{
	std::string Result(FunctionName);
    size_t Index = 0;

#ifndef WIN32 //���� �������� ������ ��� Linux - ��������� ��� ����
    Index = Result.find('('); //���� ������������� ������
    if (Index != NPOS) //���� ������ ����� - ������� ��� ��� ����� ��
    {
        Result.erase(Index, Result.size() - Index);
    }

    while ((Index = Result.find(SYMBOL_SPACE)) != NPOS)
    {
        Result.erase(0, ++Index);
    }
#endif

    Index = Result.find(':');
    if (Index != NPOS)
    {
        Result.erase(Index, Result.size() - Index);
    }
	return Result;
}
//-----------------------------------------------------------------------------
void ISAlgorithm::RemoveBeginSymbolLoop(QString &String, char Char)
{
	if (!String.isEmpty())
	{
		while (String[0] == Char)
		{
			String.remove(0, 1);
		}
	}
}
//-----------------------------------------------------------------------------
void ISAlgorithm::RemoveLastSymbolLoop(QString &String, char Char)
{
	if (!String.isEmpty())
	{
		while (String.back() == Char)
		{
			String.chop(1);
		}
	}
}
//-----------------------------------------------------------------------------
QString ISAlgorithm::GetLastErrorString()
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
bool ISAlgorithm::GenerateSalt(QString &Salt, QString &ErrorString)
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
			ErrorString = ISAlgorithm::GetLastErrorString();
		}
	}
	else //�� ������� ������� ��������
	{
		ErrorString = ISAlgorithm::GetLastErrorString();
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
		ErrorString = ISAlgorithm::GetLastErrorString();
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
QString ISAlgorithm::SaltPassword(const QString &HashPassword, const QString &Salt)
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
bool ISAlgorithm::PasswordVerification(const QString &Password)
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
void ISAlgorithm::ConvertSecondToTime(unsigned int Seconds, unsigned int &Day, unsigned int &Hour, unsigned int &Minute, unsigned int &Second)
{
	Day = Seconds / (24 * 3600);
	Seconds = Seconds % (24 * 3600);
	Hour = Seconds / 3600;
	Seconds %= 3600;
	Minute = Seconds / 60;
	Seconds %= 60;
	Second = Seconds;
}
//-----------------------------------------------------------------------------
unsigned int ISAlgorithm::ExtractVersionFile(const QString &FilePath)
{
	unsigned int Version = 0;
	QStringList StringList = QFileInfo(FilePath).completeBaseName().split('_');
	if (StringList.size() == 4) //������ ����� �������
	{
		bool Converted = false;
		Version = StringList.back().toUInt(&Converted);
		Version = Converted ? Version : 0; //�������������� ��������
	}
	return Version;
}
//-----------------------------------------------------------------------------
void ISAlgorithm::FormatPhoneNumber(QString &PhoneNumber)
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
QString ISAlgorithm::FormatNumber(long long Number, char Separator)
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
QString ISAlgorithm::FormatNumber(double Number, char Separator, unsigned int Precision)
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
std::string ISAlgorithm::GenerateUuidStandart()
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
std::string ISAlgorithm::GenerateUuid()
{
	std::string UID = ISAlgorithm::GenerateUuidStandart();
	std::transform(UID.begin(), UID.end(), UID.begin(), toupper);
	return '{' + UID + '}';
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GenerateUuidLite()
{
	std::string UID = ISAlgorithm::GenerateUuidStandart();
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
std::string ISAlgorithm::StringToSha256(const std::string &String)
{
	//��������� ���
    SHA256_CTX SHA256;
	SHA256_Init(&SHA256);
	SHA256_Update(&SHA256, String.c_str(), String.size());
	unsigned char Hash[SHA256_DIGEST_LENGTH] = { 0 };
	SHA256_Final(Hash, &SHA256);
	
	//��������������� � hex
	std::stringstream StringStream;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		StringStream << std::hex << std::setw(2) << std::setfill('0') << (int)Hash[i];
	}

	//�������� � ������, ������ ������� � ����������
	std::string Result = StringStream.str();
	std::transform(Result.begin(), Result.end(), Result.begin(), toupper);
	return Result;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::StringToMD5(const std::string &String)
{
	//��������� ���
    MD5_CTX MD5Context;
	MD5_Init(&MD5Context);
	MD5_Update(&MD5Context, String.c_str(), 5);
	unsigned char Hash[MD5_DIGEST_LENGTH] = { 0 };
	MD5_Final(Hash, &MD5Context);
	
	//��������������� � hex
	std::stringstream StringStream;
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
	{
		StringStream << std::hex << (int)Hash[i];
	}

	//�������� � ������, ������ ������� � ����������
	std::string Result = StringStream.str();
	std::transform(Result.begin(), Result.end(), Result.begin(), toupper);
	return Result;
}
//-----------------------------------------------------------------------------
QString ISAlgorithm::StringTake(QString &String, int Pos, int N)
{
	QString Result = String.mid(Pos, N);
	String.remove(Pos, N);
	return Result;
}
//-----------------------------------------------------------------------------
ISVectorString ISAlgorithm::ParseCommandArgs(int argc, char **argv)
{
	ISVectorString Vector(argc);
	for (int i = 0; i < argc; ++i)
	{
		Vector[i] = argv[i];
	}
	return Vector;
}
//-----------------------------------------------------------------------------
