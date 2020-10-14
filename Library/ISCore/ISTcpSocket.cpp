#include "ISTcpSocket.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
#include "ISLogger.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISLocalization.h"
#include "ISTcpQueue.h"
//-----------------------------------------------------------------------------
ISTcpSocket::ISTcpSocket(qintptr SocketDescriptor, QObject *parent)
	: QTcpSocket(parent),
	IsAuthorized(false)
{
	setSocketDescriptor(SocketDescriptor);
	connect(this, &ISTcpSocket::readyRead, this, &ISTcpSocket::ReadyRead);
}
//-----------------------------------------------------------------------------
ISTcpSocket::~ISTcpSocket()
{

}
//-----------------------------------------------------------------------------
void ISTcpSocket::ReadyRead()
{
	//Добавляем очередную порцию данных в буфер
	Buffer.push_back(readAll());

	long Size = 0;
	while (true) //Ждём пока не придёт запрос
	{
		ISSleep(10);
		ISSystem::ProcessEvents();
		if (!Size) //Размеры ещё не известны - вытаскиваем их
		{
			Size = ISTcp::GetQuerySizeFromBuffer(Buffer);
			if (!Size) //Если размер не удалось вытащить - вероятно пришли невалидные данные - отправляем ошибку
			{
				SendError("Not getting query size");
				ClearBuffer();
				return;
			}
		}
		if (Buffer.size() == Size) //Запрос пришёл полностью - выходим из цикла
		{
			break;
		}
	}

	QString ErrorString;
	QVariantMap VariantMap = ISSystem::JsonStringToVariantMap(Buffer, &ErrorString);
	if (VariantMap.isEmpty() && !ErrorString.isEmpty()) //При конвертации произошла ошибка
	{
		SendError(LANG("Carat.Error.ParseMessage").arg(ErrorString));
		return;
	}

	//Если поля "Type" нет - ошибка
	if (!VariantMap.contains("Type"))
	{
		SendError(LANG("Carat.Error.InvalidMessage").arg("not found field \"Type\""));
		return;
	}

	//Получаем значение поля "Type"
	QString TypeName = VariantMap["Type"].toString();

	//Если поле "Type" пустое - ошибка
	if (TypeName.isEmpty())
	{
		SendError(LANG("Carat.Error.InvalidMessage").arg("field \"Type\" is empty"));
		return;
	}

	//Получаем тип сообщения по его имени и если оно неизвестное - ошибка
	ISNamespace::ApiMessageType MessageType = GetMessageType(TypeName);
	if (MessageType == ISNamespace::AMT_Unknown)
	{
		SendError(LANG("Carat.Error.InvalidMessageType").arg(TypeName));
		return;
	}

	//Создаём и добавляем сообщение в очередь
	ISTcpQueue::Instance().AddMessage(new ISTcpMessage
	{
		MessageType,
		VariantMap["Parameters"].toMap()
	});

	//Если пришёл запрос на авторизацию и клиент уже авторизован - ошибка
	//if (QueryType == API_AUTH && IsAuthorized)
	{
		//SendError(LANG("Carat.Error.AlreadyAuth"));
		//return;
	}

	//Если пришёл не авторищационный запрос и клиент ещё не авторизован - ошибка
	//if (QueryType != API_AUTH && !IsAuthorized)
	{
		//SendError(LANG("Carat.Error.NotAuth"));
		//return;
	}

	//Если пришёл запрос на авторизацию и этот клиент ещё не авторизован - авторизуем
	//if (QueryType == API_AUTH && !IsAuthorized)
	{

	}
}
//-----------------------------------------------------------------------------
void ISTcpSocket::ClearBuffer()
{
	Buffer.clear();
}
//-----------------------------------------------------------------------------
void ISTcpSocket::Send(const QVariantMap &Data)
{
	//Если сокет все ещё подключен - отправляем
	if (state() == QTcpSocket::ConnectedState)
	{
		//Формируем ответ
		QByteArray ByteArray = ISSystem::VariantMapToJsonString(Data, QJsonDocument::Compact).toUtf8();
		ByteArray.insert(0, QString("%1.").arg(ByteArray.size()));

		//Отправляем запрос и ждём окончания его отправки
		write(ByteArray);
		ISTcp::WaitForBytesWritten(this);
	}
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SendError(const QString &error_string)
{
	//Формируем ответ с ошибкой и отправляем его
	Send(ISTcpAnswer(error_string));
	ISLOGGER_E(error_string);
}
//-----------------------------------------------------------------------------
ISNamespace::ApiMessageType ISTcpSocket::GetMessageType(const QString &TypeName) const
{
	if (TypeName == API_AUTH)
	{
		return ISNamespace::AMT_Auth;
	}
	return ISNamespace::AMT_Unknown;
}
//-----------------------------------------------------------------------------
