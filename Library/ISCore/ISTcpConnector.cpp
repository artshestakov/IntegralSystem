#include "ISTcpConnector.h"
#include "ISConstants.h"
#include "ISCountingTime.h"
#include "ISAes256.h"
#include "ISSystem.h"
#include "ISDefinesCore.h"
#include "ISSystem.h"
#include "ISTcp.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISTcpConnector::ISTcpConnector()
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	TcpSocket(new QTcpSocket(this)),
	Timer(new QTimer(this))
{
	connect(TcpSocket, &QTcpSocket::connected, &EventLoop, &QEventLoop::quit);
	connect(TcpSocket, &QTcpSocket::connected, Timer, &QTimer::stop);

	Timer->setSingleShot(true);
	Timer->setInterval(CARAT_TIMEOUT_CONNECT);
	connect(Timer, &QTimer::timeout, this, &ISTcpConnector::Timeout);
}
//-----------------------------------------------------------------------------
ISTcpConnector::~ISTcpConnector()
{

}
//-----------------------------------------------------------------------------
ISTcpConnector& ISTcpConnector::Instance()
{
	static ISTcpConnector TcpConnector;
	return TcpConnector;
}
//-----------------------------------------------------------------------------
QString ISTcpConnector::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
QTcpSocket* ISTcpConnector::GetSocket()
{
	return TcpSocket;
}
//-----------------------------------------------------------------------------
const std::vector<unsigned char> ISTcpConnector::GetToken() const
{
	return Token;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::IsConnected() const
{
	return TcpSocket->state() == QTcpSocket::ConnectedState;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Reconnect(const QString &Host, quint16 Port)
{
	if (IsConnected())
	{
		Disconnect();
	}
	return Connect(Host, Port);
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Connect(const QString &Host, quint16 Port)
{
	Timer->start();
	TcpSocket->connectToHost(Host, Port);
	EventLoop.exec();
	bool Result = IsConnected();
	if (Result)
	{
		if (Token.empty()) //Если токен ещё не существует - генерируем его
		{
#ifdef DEBUG
			Token = std::vector<unsigned char>(CARAT_TOKEN_TEST, CARAT_TOKEN_TEST + CARAT_TOKEN_SIZE);
#else
			Result = CreateToken();
			if (Result) //Токен успешно сгенерирован - отправляем его
			{
				Result = SendToken();
				if (Result) //Если токен отправлен - немного ждём
				{
					QEventLoop EventLoop;
					QTimer::singleShot(500, &EventLoop, &QEventLoop::quit);
					EventLoop.exec();
				}
			}
			else //Ошибка генерации токена - обрываем соединение
			{
				Disconnect();
			}
#endif
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Disconnect()
{
	if (IsConnected())
	{
		TcpSocket->disconnectFromHost();
	}
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Timeout()
{
	//Подключение не произошло - ошибка
	if (TcpSocket->state() != QTcpSocket::ConnectedState)
	{
		ErrorString = "Connecting timeout";
		EventLoop.quit();
	}
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Error(QTcpSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
	ErrorString = TcpSocket->errorString();
	EventLoop.quit();
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::CreateToken()
{
	HINSTANCE HModule = LoadLibrary(ISDefines::Core::PATH_LIB_CRYPTER.toStdString().c_str());
	if (HModule == NULL) //Ошибка загрузки библиотеки
	{
		ErrorString = "Error loading crypt module";
		return false;
	}

	//Определение функций шифрования и получения ошибки
	typedef int(__stdcall *GenerateImage)(const char *, int, int, const char *);
	typedef const char *(__stdcall *GetError)(void);

	//Создание экземпляров функций
	GenerateImage generate_image = (GenerateImage)GetProcAddress(HModule, "GenerateImage");
	GetError get_error = (GetError)GetProcAddress(HModule, "GetError");

	if (!generate_image || !get_error) //Функции библиотеки определены неправильно
	{
		ErrorString = "Error function address with name";
		return false;
	}

	//Генерируем второй токен
	std::string TokenString = ISSystem::GenerateUuid().remove('-').toLower().toStdString();
	TokenString = TokenString.substr(1, TokenString.size() - 2);
	Token = std::vector<unsigned char>(TokenString.begin(), TokenString.end());

	PathToken = ISDefines::Core::PATH_APPLICATION_DIR + "/token";
	if (generate_image(PathToken.toStdString().c_str(), CARAT_TOKEN_WIDTH, CARAT_TOKEN_HEIGHT, TokenString.c_str()) == 0) //Ошибка шифрования
	{
		ErrorString = get_error();
	}
	FreeLibrary(HModule);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::SendToken()
{
	QFile File(PathToken);
	if (!File.open(QIODevice::ReadOnly))
	{
		ErrorString = "Error read token: " + File.errorString();
		return false;
	}

	QByteArray TokenData = File.readAll();
	File.close();

	quint64 WritedSize = TcpSocket->write(TokenData);
	if (WritedSize != TokenData.size())
	{
		ErrorString = "Error sending token: sended size not equal token data size";
		return false;
	}
	
	//Ждём пока данные не будут полностью записаны в сокет
	ISTcp::WaitForBytesWritten(TcpSocket);
	return true;
}
//-----------------------------------------------------------------------------
