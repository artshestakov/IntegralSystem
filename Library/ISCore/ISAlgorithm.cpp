#include "ISConstants.h"
#include "ISAlgorithm.h"
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
qint64 ISAlgorithm::GetFileSize(const std::string &FilePath)
{
	std::string ErrorString;
	return ISAlgorithm::GetFileSize(FilePath, ErrorString);
}
//-----------------------------------------------------------------------------
qint64 ISAlgorithm::GetFileSize(const std::string &FilePath, std::string &ErrorString)
{
	FILE *File = fopen(FilePath.c_str(), "r");
	if (File)
	{
		fseek(File, 0L, SEEK_END);
        qint64 FileSize = ftell(File);
		fclose(File);
		return FileSize;
	}
	else
	{
		ErrorString = strerror(errno);
	}
	return -1;
}
//-----------------------------------------------------------------------------
ISTimePoint ISAlgorithm::GetTick()
{
	return std::chrono::steady_clock::now();
}
//-----------------------------------------------------------------------------
long long ISAlgorithm::GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(T1 - T2).count();
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetClassName(const std::string &FunctionName)
{
	std::string Result(FunctionName);
    size_t Index = 0;

#ifndef WIN32 //Если работаем сейчас под Linux - исключаем имя типа
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
	if (Result != 0) //Получение описания ошибки прошло успешно
	{
		ErrorString = QString::fromLocal8Bit(Buffer, (int)Result);
	}
	else //Ошибка
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
	//Объявляем результирующую строку и буфер
	unsigned char Buffer[CARAT_SALT_SIZE] = { 0 };
#ifdef WIN32 //Формирование соли под Windows
	HCRYPTPROV CryptoProvider = 0;
	bool Result = CryptAcquireContext(&CryptoProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT) == TRUE;
	if (Result) //Контекст создан успешно - формируем соль
	{
		Result = CryptGenRandom(CryptoProvider, CARAT_SALT_SIZE, Buffer) == TRUE;
		if (Result) //Генерация прошла успешно - освобождаем контекст
		{
			CryptReleaseContext(CryptoProvider, 0);
		}
		else //Ошибка генерации
		{
			ErrorString = ISAlgorithm::GetLastErrorString();
		}
	}
	else //Не удалось создать контекст
	{
		ErrorString = ISAlgorithm::GetLastErrorString();
	}
#else //Формирование соли под Linux
	FILE *FileDevice = fopen("/dev/random", "r");
	bool Result = FileDevice ? true : false;
	if (Result) //Устройство удалось открыть - читаем и закрываем устройство
	{
		Result = fread(&Buffer[0], sizeof(char), CARAT_SALT_SIZE, FileDevice) == CARAT_SALT_SIZE;
		fclose(FileDevice);
	}
	else
	{
		ErrorString = ISAlgorithm::GetLastErrorString();
	}
#endif
	if (Result) //Если все хорошо - формируем соль в HEX
	{
		for (unsigned long i = 0; i < CARAT_SALT_SIZE; ++i) //Обходим буфер с солью
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
	//Если длинна пароля маленькая - ошибка
	if (Password.size() < MINIMUM_PASSWORD_LENGHT)
	{
		return false;
	}

	//Обходим строку
	for (const char &Char : Password.toStdString())
	{
		if (Char >= 33 && Char <= 126) //Если текущий символ входит в диапазон согласно ACII-таблице - все окей
		{
			continue;
		}
		return false; //Иначе - ошибка
	}
	return true;
	/*int Pos = 0;
	QString Temp = Password;
	return QRegExpValidator(QRegExp("^([0-9A-Za-z!\"#$%&'()*+,-.\/:;<=>?@[\\\]^_`{|}~]{8,})$")).validate(Temp, Pos) == QRegExpValidator::Acceptable;*/
}
//-----------------------------------------------------------------------------
