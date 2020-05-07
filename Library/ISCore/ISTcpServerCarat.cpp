#include "ISTcpServerCarat.h"
#include "ISTcp.h"
#include "ISTcpAnswer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpServerCarat::ISTcpServerCarat(QObject *parent) : ISTcpServerBase(parent)
{
	
}
//-----------------------------------------------------------------------------
ISTcpServerCarat::~ISTcpServerCarat()
{
	
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::incomingConnection(qintptr SocketDescriptor)
{
	ISTcpServerBase::incomingConnection(SocketDescriptor);
	QTcpSocket *TcpSocket = nextPendingConnection();
	connect(TcpSocket, &QTcpSocket::disconnected, TcpSocket, &QTcpSocket::deleteLater);

	//Ждём пока не придёт запрос
	QByteArray ByteArray;
	while (true)
	{
		TcpSocket->waitForReadyRead();
		if (TcpSocket->bytesAvailable() > 0)
		{
			ByteArray.append(TcpSocket->readAll());
			if (ByteArray.right(CARAT_PACKET_SEPARATOR_SIZE) == CARAT_PACKET_SEPARATOR)
			{
				ByteArray.remove(ByteArray.size() - CARAT_PACKET_SEPARATOR_SIZE, CARAT_PACKET_SEPARATOR_SIZE);
				break;
			}
		}
	}
	
	//Проверка валидности запроса
	QVariantMap VariantMap;
	QString ErrorString;
	if (!ISTcp::IsValidQuery(ByteArray, VariantMap, ErrorString))
	{
		SendError(TcpSocket, ErrorString);
		return;
	}

	//Проверка типа запроса
	QString QueryType = VariantMap["Type"].toString();
	if (QueryType != API_AUTH) //Если не авторизация - ошибка
	{
		SendError(TcpSocket, QString("Invalid query type \"%1\"").arg(QueryType));
		return;
	}

	VariantMap = VariantMap["Parameters"].toMap();
	QString Login = VariantMap["Login"].toString();
	QString Password = VariantMap["Password"].toString();

	//Тестовая авторизация
	ISTcpAnswer TcpAnswer;
	if (Login == "postgres" && Password == "adm777") //Логин и пароль верные
	{
		Send(TcpSocket, TcpAnswer);
	}
	else //Логин и пароль неверные
	{
		SendError(TcpSocket, "Invalid login or password");
	}
}
//-----------------------------------------------------------------------------
void ISTcpServerCarat::SendError(QTcpSocket *TcpSocket, const QString &ErrorString)
{
	//Формируем ответ с ошибкой
	ISTcpAnswer TcpAnswer;
	TcpAnswer.SetError(ErrorString);

	//Отправляем и обрываем соединение
	Send(TcpSocket, TcpAnswer);
	TcpSocket->abort();
}
//-----------------------------------------------------------------------------
