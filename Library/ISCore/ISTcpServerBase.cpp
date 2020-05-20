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
	//���� ����� ��� ��� ��������� - ����������
	if (TcpSocket->state() == QTcpSocket::ConnectedState)
	{
		QByteArray ByteArray;
		if (Token.empty()) //���� ����� ��� - �� �������
		{
			ByteArray = ISSystem::VariantMapToJsonString(Data).toUtf8();
			int Size = ByteArray.size();
			ByteArray.insert(0, SYMBOL_POINT);
			ByteArray.insert(0, QString::number(Size));
		}
		else //����� �������
		{
			ByteArray = ISTcp::Crypt(Token, Data);
		}

		//�������� �������
		TcpSocket->write(ByteArray);
		TcpSocket->flush();

		//��� ���� ������ �����
		TcpSocket->waitForBytesWritten();
	}
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::SendError(QTcpSocket *TcpSocket, const QString &ErrorString)
{
	//��������� ����� � �������
	ISTcpAnswer TcpAnswer;
	TcpAnswer.SetError(ErrorString);

	//���������� � �������� ����������
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
