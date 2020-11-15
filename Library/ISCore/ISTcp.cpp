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
		ISSystem::ProcessEvents();
		ISSleep(1);
	}
}
//-----------------------------------------------------------------------------
void ISTcp::WaitForDisconnected(QTcpSocket *TcpSocket)
{
	while (TcpSocket->state() == QAbstractSocket::ConnectedState)
	{
		ISSystem::ProcessEvents();
		ISSleep(1);
	}
}
//-----------------------------------------------------------------------------
