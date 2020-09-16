#include "ISTcpSocket.h"
//-----------------------------------------------------------------------------
ISTcpSocket::ISTcpSocket(qintptr SocketDescriptor, QObject *parent) : QTcpSocket(parent)
{
	setSocketDescriptor(SocketDescriptor);
}
//-----------------------------------------------------------------------------
ISTcpSocket::~ISTcpSocket()
{

}
//-----------------------------------------------------------------------------
void ISTcpSocket::ReadyRead()
{
	Buffer.push_back(readAll());
}
//-----------------------------------------------------------------------------
