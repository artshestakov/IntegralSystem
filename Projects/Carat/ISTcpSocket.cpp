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
ISTcpSocket::ISTcpSocket(qintptr SocketDescriptor, QObject *parent)
	: QTcpSocket(parent),
	MessageSize(0),
	ChunkCount(0),
	IsAuthorized(false),
	UserID(0)
{
	setSocketDescriptor(SocketDescriptor);

	Timer = new QTimer(this);
	Timer->setInterval(5000);
	Timer->setSingleShot(true);
	connect(Timer, &QTimer::timeout, this, &ISTcpSocket::abort);

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
void ISTcpSocket::SetUserID(int user_id)
{
	UserID = user_id;
}
//-----------------------------------------------------------------------------
int ISTcpSocket::GetUserID() const
{
	return UserID;
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
			ClearBuffer();
			return;
		}
	}

	//Инкрементируем количество чанков
	++ChunkCount;

	//Проверяем, не пришло ли сообщение полностью - выходим если пришло не полностью
	if (Buffer.size() != MessageSize)
	{
		return;
	}

	//Останавливаем таймер
	Timer->stop();

	//Создаём указатель на сообщение
	ISTcpMessage *TcpMessage = new ISTcpMessage(this);
	TcpMessage->Size = Buffer.size();
	TcpMessage->ChunkCount = ChunkCount;

	//Засекаем время и парсим сообщение
	QJsonParseError JsonParseError;
	ISTimePoint TimePoint = ISAlgorithm::GetTick();
	TcpMessage->Parameters = ISSystem::JsonStringToVariantMap(Buffer, JsonParseError);
	TcpMessage->ParseMSec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);

	bool Result = !TcpMessage->Parameters.isEmpty() && JsonParseError.error == QJsonParseError::NoError;
	if (Result) //Конвертация прошла успешно
	{
		Result = TcpMessage->Parameters.contains("Type");
		if (Result) //Если поле "Type" есть
		{
			//Получаем значение поля "Type"
			TcpMessage->TypeName = TcpMessage->Parameters["Type"].toString();
			Result = !TcpMessage->TypeName.isEmpty();
			if (Result) //Если поле "Type" не пустое
			{
				//Получаем тип сообщения по его имени и если оно неизвестное - ошибка
				ISNamespace::ApiMessageType MessageType = GetMessageType(TcpMessage->TypeName);
				Result = MessageType != ISNamespace::AMT_Unknown;
				if (Result) //Сообщение валидное
				{
					TcpMessage->Type = MessageType;
					TcpMessage->Parameters = TcpMessage->Parameters["Parameters"].toMap();
				}
				else //Тип сообщения не известный
				{
					TcpMessage->SetErrorString(LANG("Carat.Error.InvalidMessageType").arg(TcpMessage->TypeName));
				}
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
ISNamespace::ApiMessageType ISTcpSocket::GetMessageType(const QString &TypeName) const
{
	if (TypeName == API_AUTH)
	{
		return ISNamespace::AMT_Auth;
	}
	else if (TypeName == API_SLEEP)
	{
		return ISNamespace::AMT_Sleep;
	}
	else if (TypeName == API_GET_META_DATA)
	{
		return ISNamespace::AMT_GetMetaData;
	}
	return ISNamespace::AMT_Unknown;
}
//-----------------------------------------------------------------------------
void ISTcpSocket::ClearBuffer()
{
	Buffer.clear();
	MessageSize = 0;
	ChunkCount = 0;
}
//-----------------------------------------------------------------------------
