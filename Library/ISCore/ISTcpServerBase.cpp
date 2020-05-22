#include "ISTcpServerBase.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISTcpAnswer.h"
#include "ISLogger.h"
#include "ISTcp.h"
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
void ISTcpServerBase::SetToken(const std::string &token)
{
	Token = std::vector<unsigned char>(token.begin(), token.end());
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
void ISTcpServerBase::Send(QTcpSocket *TcpSocket, const QVariantMap &Data)
{
	//���� ����� ��� ��� ��������� - ����������
	if (TcpSocket->state() == QTcpSocket::ConnectedState)
	{
		//������� ������
		QByteArray ByteArray = ISTcp::Crypt(Token, Data);

		//�������� �������
		TcpSocket->write(ByteArray);

		//��� ���� ������ �� ����� ��������� �������� � �����
		ISTcp::WaitForBytesWritten(TcpSocket);
	}
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::SendError(QTcpSocket *TcpSocket, const QString &ErrorString)
{
	//��������� ����� � �������
	ISTcpAnswer TcpAnswer;
	TcpAnswer.SetError(ErrorString);

	//���������� � �������� ����������
	Send(TcpSocket, TcpAnswer);
	TcpSocket->abort();
	ISLOGGER_E(ErrorString);
}
//-----------------------------------------------------------------------------
std::vector<unsigned char>& ISTcpServerBase::GetToken()
{
	return Token;
}
//-----------------------------------------------------------------------------
QString ISTcpServerBase::GetTokenString() const
{
	QString TokenString(CARAT_TOKEN_SIZE, Qt::Uninitialized);
	for (size_t i = 0; i < CARAT_TOKEN_SIZE; ++i)
	{
		TokenString[i] = Token[i];
	}
	return TokenString;
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::AcceptError(QTcpSocket::SocketError)
{
	ErrorString = errorString();
}
//-----------------------------------------------------------------------------
