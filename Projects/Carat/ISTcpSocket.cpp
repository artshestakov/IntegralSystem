#include "ISTcpSocket.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
#include "ISLogger.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISLocalization.h"
#include "ISTcpQueue.h"
#include "ISTcpMessage.h"
//-----------------------------------------------------------------------------
ISTcpSocket::ISTcpSocket(qintptr socket_descriptor, QObject *parent)
	: QTcpSocket(parent),
	SocketDescriptor(socket_descriptor),
	MessageSize(0),
	ChunkCount(0),
	IsAuthorized(false),
	UserID(0),
	UserIsSystem(false)
{
	setSocketDescriptor(SocketDescriptor);
	Address = peerAddress().toString(); //Адрес можно получить только после установки дескриптоба базовому классу

	Timer = new QTimer(this);
	Timer->setInterval(5000);
	Timer->setSingleShot(true);
	connect(Timer, &QTimer::timeout, this, &ISTcpSocket::Timeout);

	//Эти сигналы обязательно должны подключаться в конце конструктора
	connect(this, static_cast<void(ISTcpSocket::*)(QAbstractSocket::SocketError)>(&ISTcpSocket::error), this, &ISTcpSocket::Error);
	connect(this, &ISTcpSocket::readyRead, this, &ISTcpSocket::ReadyRead);
}
//-----------------------------------------------------------------------------
ISTcpSocket::~ISTcpSocket()
{
	if (Timer->isActive()) //Если таймер был запущен - останавливаем его
	{
		Timer->stop();
	}
}
//-----------------------------------------------------------------------------
qintptr ISTcpSocket::GetSocketDescriptor() const
{
	return SocketDescriptor;
}
//-----------------------------------------------------------------------------
QString ISTcpSocket::GetAddress() const
{
	return Address;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SetAuthorized(bool authorized)
{
	IsAuthorized = authorized;
}
//-----------------------------------------------------------------------------
bool ISTcpSocket::GetAuthorized() const
{
	return IsAuthorized;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SetUserID(unsigned int user_id)
{
	UserID = user_id;
}
//-----------------------------------------------------------------------------
unsigned int ISTcpSocket::GetUserID() const
{
	return UserID;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SetUserIsSystem(bool user_is_system)
{
	UserIsSystem = user_is_system;
}
//-----------------------------------------------------------------------------
bool ISTcpSocket::GetUserIsSystem() const
{
	return UserIsSystem;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::ReadyRead()
{
	//Добавляем очередную порцию данных в буфер, перезапускаем таймер и проверяем размер
	Buffer.push_back(readAll());
	Timer->start(); 
	if (!MessageSize) //Если размер сообщения ещё не известен
	{
		bool Ok = true;
		MessageSize = ISTcp::GetQuerySizeFromBuffer(Buffer, Ok);
		if (!Ok) //Если не удалось вытащить размер сообщения - принудительно отключаем клиента, сообщаем об ошибке и очищаем буфер
		{
			abort();
			ISLOGGER_E(__CLASS__, "Not get message size. Client will be disconnected. Invalid message:\n" + Buffer);
			return;
		}
	}

	//Инкрементируем количество чанков и проверяем, не пришло ли сообщение полностью
	++ChunkCount;
	if (Buffer[Buffer.size() - 1] != SYMBOL_NULL_TERM)
	{
		return;
	}

	//Сообщение пришло полностью, теперь сверяем размер
	//Если размеры отличаются, значит что-то не так и отключаем клиента принудительно
	if ((unsigned int)Buffer.size() != MessageSize)
	{
		ISLOGGER_E(__CLASS__, QString("Invalid size. Declared size %1, read size %2").arg(MessageSize).arg(Buffer.size()));
		abort();
		ClearBuffer();
	}

	//Останавливаем таймер
	Timer->stop();

	//Создаём указатель на сообщение
	ISTcpMessage *TcpMessage = new ISTcpMessage(this);
	TcpMessage->Size = Buffer.size();
	TcpMessage->ChunkCount = ChunkCount;

	//Регистрируем входящий траффик
	//ISCaratMonitor::Instance().IncomingQuerySize(TcpMessage->Size);

	//Засекаем время и парсим сообщение
	QJsonParseError JsonParseError;
	ISTimePoint TimePoint = ISAlgorithm::GetTick();
	QVariantMap VariantMap = ISSystem::JsonStringToVariantMap(Buffer, JsonParseError);
	TcpMessage->ParseMSec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);

	bool Result = !VariantMap.isEmpty() && JsonParseError.error == QJsonParseError::NoError;
	if (Result) //Конвертация прошла успешно
	{
		Result = VariantMap.contains("Type");
		if (Result) //Если поле "Type" есть
		{
			//Получаем значение поля "Type"
			TcpMessage->Type = VariantMap["Type"].toString();
			Result = !TcpMessage->Type.isEmpty();
			if (Result) //Если поле "Type" не пустое
			{
				TcpMessage->Parameters = VariantMap["Parameters"].toMap();
			}
			else //Поле "Type" пустое
			{
				TcpMessage->SetErrorString(LANG("Carat.Error.InvalidMessage").arg("field \"Type\" is empty"));
			}
		}
		else //Поле "Type" отсутствует
		{
			TcpMessage->SetErrorString(LANG("Carat.Error.InvalidMessage").arg("not found field \"Type\""));
		}
	}
	else //Ошибка при конвертации сообщения
	{
		TcpMessage->SetErrorString(LANG("Carat.Error.ParseMessage").arg(JsonParseError.errorString()));
	}
	
	//Очищаем буфер и добавляем сообщение в очередь
	ClearBuffer();
	ISTcpQueue::Instance().AddMessage(TcpMessage);
}
//-----------------------------------------------------------------------------
void ISTcpSocket::Error(QAbstractSocket::SocketError socket_error)
{
	if (socket_error != QAbstractSocket::RemoteHostClosedError)
	{
		ISLOGGER_E(__CLASS__, errorString());
	}
}
//-----------------------------------------------------------------------------
void ISTcpSocket::ClearBuffer()
{
	Buffer.clear();
	MessageSize = 0;
	ChunkCount = 0;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::Timeout()
{
	ISLOGGER_E(__CLASS__, "Message come not complete from " + peerAddress().toString());
	abort();
}
//-----------------------------------------------------------------------------
