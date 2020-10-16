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
ISTcpSocket::ISTcpSocket(qintptr SocketDescriptor, QObject *parent) : QTcpSocket(parent)
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
		ISSleep(1);
		ISSystem::ProcessEvents();
		if (!Size) //Размеры ещё не известны - вытаскиваем их
		{
			Size = ISTcp::GetQuerySizeFromBuffer(Buffer);
			if (!Size) //Если размер не удалось вытащить - вероятно пришли невалидные данные - отправляем ошибку
			{
				SendErrorQuery(LANG("Carat.Error.MessageSize"));
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
		SendErrorQuery(LANG("Carat.Error.ParseMessage").arg(ErrorString));
		return;
	}

	//Если поля "Type" нет - ошибка
	if (!VariantMap.contains("Type"))
	{
		SendErrorQuery(LANG("Carat.Error.InvalidMessage").arg("not found field \"Type\""));
		return;
	}

	//Получаем значение поля "Type"
	QString TypeName = VariantMap["Type"].toString();

	//Если поле "Type" пустое - ошибка
	if (TypeName.isEmpty())
	{
		SendErrorQuery(LANG("Carat.Error.InvalidMessage").arg("field \"Type\" is empty"));
		return;
	}

	//Получаем тип сообщения по его имени и если оно неизвестное - ошибка
	ISNamespace::ApiMessageType MessageType = GetMessageType(TypeName);
	if (MessageType == ISNamespace::AMT_Unknown)
	{
		SendErrorQuery(LANG("Carat.Error.InvalidMessageType").arg(TypeName));
		return;
	}

	Buffer.clear(); //Очищаем буфер

	//Создаём и добавляем сообщение в очередь
	ISTcpQueue::Instance().AddMessage(new ISTcpMessage
	{
		MessageType,
		VariantMap["Parameters"].toMap(),
		this
	});
}
//-----------------------------------------------------------------------------
void ISTcpSocket::SendErrorQuery(const QString &ErrorString)
{
	Buffer.clear(); //Очищаем буфер

	//Создаём ошибочное сообщение и передаём его в очередь через стандартный механизм
	ISTcpQueue::Instance().AddMessage(new ISTcpMessage
	{
		this,
		ErrorString
	});
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
