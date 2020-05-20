#include "ISTcpServerBase.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISTcpAnswer.h"
#include "ISLogger.h"
#include "ISTcp.h"
//-----------------------------------------------------------------------------
ISTcpServerBase::ISTcpServerBase(QObject *parent)
	: QTcpServer(parent),
	ErrorString(NO_ERROR_STRING),
	ModeTest(false)
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
void ISTcpServerBase::SetModeTest(bool mode_test)
{
	ModeTest = mode_test;
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
void ISTcpServerBase::SetErrorString(const QString &error_string)
{
	ErrorString = error_string;
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::Send(QTcpSocket *TcpSocket, std::vector<unsigned char> &Token, const QVariantMap &Data)
{
	//Если сокет все ещё подключен - отправляем
	if (TcpSocket->state() == QTcpSocket::ConnectedState)
	{
		QByteArray ByteArray;
		if (Token.empty()) //Если ключа нет - не шифруем
		{
			ByteArray = ISSystem::VariantMapToJsonString(Data).toUtf8();
			int Size = ByteArray.size();
			ByteArray.insert(0, SYMBOL_POINT);
			ByteArray.insert(0, QString::number(Size));
		}
		else //Иначе шифруем
		{
			ByteArray = ISTcp::Crypt(Token, Data);
		}

		//Отправка запроса
		TcpSocket->write(ByteArray);
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
	Send(TcpSocket, std::vector<unsigned char>(), TcpAnswer);
	TcpSocket->abort();
	ISLOGGER_E(ErrorString);
}
//-----------------------------------------------------------------------------
bool ISTcpServerBase::IsModeTest() const
{
	return ModeTest;
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::AcceptError(QTcpSocket::SocketError)
{
	ErrorString = errorString();
}
//-----------------------------------------------------------------------------
