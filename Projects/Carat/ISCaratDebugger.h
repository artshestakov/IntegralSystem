#pragma once
#ifndef _ISCARATDEBUGGER_H_INCLUDED
#define _ISCARATDEBUGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCaratDebugger : public QObject
{
	Q_OBJECT

public:
	ISCaratDebugger(QObject *parent = 0);
	virtual ~ISCaratDebugger();

private:
	void ReadyRead();
	void Disconnected();
	void Error(QLocalSocket::LocalSocketError ErrorType);
	void StateChanged(QLocalSocket::LocalSocketState NewState);
	void Connect();
	void TimeoutConnect();

private:
	QTimer *Timer;
	QLocalSocket *LocalSocket;
};
//-----------------------------------------------------------------------------
#endif
