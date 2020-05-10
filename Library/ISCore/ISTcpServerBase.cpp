#include "ISTcpServerBase.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISTcpAnswer.h"
//-----------------------------------------------------------------------------
ISTcpServerBase::ISTcpServerBase(QObject *parent)
	: QTcpServer(parent),
	ErrorString(NO_ERROR_STRING)
{
	setMaxPendingConnections(1);
}
//-----------------------------------------------------------------------------
ISTcpServerBase::~ISTcpServerBase()
{

}
//-----------------------------------------------------------------------------
QString ISTcpServerBase::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISTcpServerBase::Run(quint16 Port)
{
	bool Result = listen(QHostAddress::AnyIPv4, Port);
	if (Result)
	{
		connect(this, &ISTcpServerBase::acceptError, this, &ISTcpServerBase::AcceptError);
	}
	else
	{
		ErrorString = errorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::Send(QTcpSocket *TcpSocket, const QVariantMap &Data)
{
	//Если сокет все ещё подключен - отправляем
	if (TcpSocket->state() == QAbstractSocket::ConnectedState)
	{
		//Сборка запроса
		QString String = ISSystem::VariantMapToJsonString(Data).simplified();
		String.insert(0, QString::number(String.size()));

		//Отправка запроса
		TcpSocket->write(String.toUtf8());
		TcpSocket->flush();

		//Ждём пока данные уйдут
		TcpSocket->waitForBytesWritten();
	}
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::SendError(QTcpSocket *TcpSocket, const QString &ErrorString)
{
	//Формируем ответ с ошибкой
	ISTcpAnswer TcpAnswer;
	TcpAnswer.SetError(ErrorString);

	//Отправляем и обрываем соединение
	Send(TcpSocket, TcpAnswer);
	TcpSocket->abort();
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::AcceptError(QAbstractSocket::SocketError)
{
	ErrorString = errorString();
}
//-----------------------------------------------------------------------------
