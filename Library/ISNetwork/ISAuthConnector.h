#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISAuthConnector : public QTcpSocket
{
	Q_OBJECT

signals:
	void ConnectedToHost();
	void ConnectedFailed();

public:
	ISAuthConnector(QObject *parent = 0);
	virtual ~ISAuthConnector();

	void Connect();
	void Disconnect();

protected:
	void Connected();
	void Error(QAbstractSocket::SocketError SocketError);
};
//-----------------------------------------------------------------------------
