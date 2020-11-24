#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
unsigned int ISTcp::GetQuerySizeFromBuffer(QByteArray &ByteArray, bool &Ok)
{
    int Pos = ByteArray.indexOf(SYMBOL_POINT);
	Ok = Pos != -1;
	if (Ok)
	{
		QByteArray ByteArraySize = ByteArray.mid(0, Pos);
		unsigned int Result = ByteArraySize.toUInt(&Ok);
		if (Ok)
		{
			ByteArray.remove(0, Pos + 1);
			return Result;
		}
	}
	return 0;
}
//-----------------------------------------------------------------------------
void ISTcp::WaitForBytesWritten(QTcpSocket *TcpSocket)
{
	while (TcpSocket->bytesToWrite() > 0)
	{
		PROCESS_EVENTS();
		ISSleep(1);
	}
}
//-----------------------------------------------------------------------------
void ISTcp::WaitForDisconnected(QTcpSocket *TcpSocket)
{
	while (TcpSocket->state() == QAbstractSocket::ConnectedState)
	{
		PROCESS_EVENTS();
		ISSleep(1);
	}
}
//-----------------------------------------------------------------------------
ISNamespace::ApiMessageType ISTcp::GetMessageTypeByName(const QString &Name)
{
	if (Name == API_AUTH)
	{
		return ISNamespace::AMT_Auth;
	}
	else if (Name == API_SLEEP)
	{
		return ISNamespace::AMT_Sleep;
	}
	else if (Name == API_GET_META_DATA)
	{
		return ISNamespace::AMT_GetMetaData;
	}
	else if (Name == API_GET_LAST_CLIENT)
	{
		return ISNamespace::AMT_GetLastClient;
	}
	else if (Name == API_USER_PASSWORD_EXIST)
	{
		return ISNamespace::AMT_UserPasswordExist;
	}
	else if (Name == API_USER_PASSWORD_CREATE)
	{
		return ISNamespace::AMT_UserPasswordCreate;
	}
	else if (Name == API_USER_PASSWORD_EDIT)
	{
		return ISNamespace::AMT_UserPasswordEdit;
	}
	else if (Name == API_USER_PASSWORD_RESET)
	{
		return ISNamespace::AMT_UserPasswordReset;
	}
	else if (Name == API_GET_RECORD_CALL)
	{
		return ISNamespace::AMI_GetRecordCall;
	}
	return ISNamespace::AMT_Unknown;
}
//-----------------------------------------------------------------------------
