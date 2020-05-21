#include "ISTcpConnector.h"
#include "ISConstants.h"
#include "ISCountingTime.h"
#include "ISAes256.h"
#include "ISSystem.h"
#include "ISDefinesCore.h"
#include "ISSystem.h"
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
bool ISTcpConnector::Reconnect(const QString &Host, quint16 Port, const QString &Login, const QString &Password)
{
	if (IsConnected())
	{
		Disconnect();
	}
	return Connect(Host, Port, Login, Password);
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Connect(const QString &Host, quint16 Port, const QString &Login, const QString &Password)
{
	Timer->start();
	TcpSocket->connectToHost(Host, Port);
	EventLoop.exec();
	bool Result = IsConnected();
	if (Result)
	{
		if (Token.empty())
		{
			CreateToken(Login, Password);
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
bool ISTcpConnector::CreateToken(const QString &Login, const QString &Password)
{
	QString TokenString = ISSystem::StringToMD5(ISSystem::StringToMD5(Login + QString::number(TcpSocket->localPort())) + ISSystem::StringToMD5(Password + Login));
	std::string TokenSTD = TokenString.toStdString();
	Token = std::vector<unsigned char>(TokenSTD.begin(), TokenSTD.end());

	HINSTANCE HModule = LoadLibrary("C:\\Github\\Crypter\\Bin\\Release-Win32\\libCrypter.dll");
	if (HModule == NULL) //Ошибка загрузки библиотеки
	{
		ErrorString = "Error loading crypt module";
		return false;
	}

	//Определение функций шифрования и получения ошибки
	typedef int(__stdcall *GenerateImage)(const char *, const char *);
	typedef const char *(__stdcall *GetError)(void);

	//Создание экземпляров функций
	GenerateImage generate_image = (GenerateImage)GetProcAddress(HModule, "GenerateImage");
	GetError get_error = (GetError)GetProcAddress(HModule, "GetError");

	if (!generate_image || !get_error) //Функции библиотеки определены неправильно
	{
		ErrorString = "Error function name";
		return false;
	}

	//Генерируем второй токен
	Token2 = ISSystem::GenerateUuid().toLower();
	Token2 = Token2.mid(1, Token2.size() - 2);

	if (generate_image((ISDefines::Core::PATH_APPLICATION_DIR + "/token").toStdString().c_str(), Token2.toStdString().c_str()) == 0) //Ошибка шифрования
	{
		ErrorString = get_error();
	}
	FreeLibrary(HModule);
	return true;
}
//-----------------------------------------------------------------------------
