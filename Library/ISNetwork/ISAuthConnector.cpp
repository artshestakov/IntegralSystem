#include "StdAfx.h"
#include "ISAuthConnector.h"
#include "EXConstants.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISAuthConnector::ISAuthConnector(QObject *parent) : QTcpSocket(parent)
{
	connect(this, &ISAuthConnector::connected, this, &ISAuthConnector::Connected);
	connect(this, static_cast<void(ISAuthConnector::*)(QAbstractSocket::SocketError)>(&ISAuthConnector::error), this, &ISAuthConnector::Error);
}
//-----------------------------------------------------------------------------
ISAuthConnector::~ISAuthConnector()
{

}
//-----------------------------------------------------------------------------
void ISAuthConnector::Connect()
{
	connectToHost(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT));
}
//-----------------------------------------------------------------------------
void ISAuthConnector::Disconnect()
{
	disconnectFromHost();
}
//-----------------------------------------------------------------------------
void ISAuthConnector::Connected()
{
	emit ConnectedToHost();
}
//-----------------------------------------------------------------------------
void ISAuthConnector::Error(QAbstractSocket::SocketError SocketError)
{
    Q_UNUSED(SocketError);
	emit ConnectedFailed();
}
//-----------------------------------------------------------------------------
