#include "ISTcpSocket.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
#include "ISLogger.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISTcpSocket::ISTcpSocket(qintptr SocketDescriptor, QObject *parent)
	: QTcpSocket(parent),
	IsAuthorized(false)
{
	Functions[API_AUTH] = &ISTcpSocket::Auth;

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

	//Проверка валидности запроса
	QVariantMap VariantMap;
	QString ErrorString, QueryType;
	if (!ISTcp::IsValidQuery(Buffer, VariantMap, ErrorString, QueryType)) //Ошибка парсинга
	{
		SendError(LANG("Carat.Error.ParseQuery").arg(ErrorString));
		return;
	}

	//Если пришёл запрос на авторизацию и клиент уже авторизован - ошибка
	if (QueryType == API_AUTH && IsAuthorized)
	{
		SendError(LANG("Carat.Error.AlreadyAuth"));
		return;
	}

	//Если пришёл не авторищационный запрос и клиент ещё не авторизован - ошибка
	if (QueryType != API_AUTH && !IsAuthorized)
	{
		SendError(LANG("Carat.Error.NotAuth"));
		return;
	}

	//Если пришёл запрос на авторизацию и этот клиент ещё не авторизован - авторизуем
	if (QueryType == API_AUTH && !IsAuthorized)
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
void ISTcpSocket::Auth(const QVariantMap &VariantMap)
{

}
//-----------------------------------------------------------------------------
