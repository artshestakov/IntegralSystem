#pragma once
#ifndef _ISTCPSERVERBASE_H_INCLUDED
#define _ISTCPSERVERBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServerBase : public QTcpServer
{
	Q_OBJECT

public:
	ISTcpServerBase(QObject *parent = 0);
	virtual ~ISTcpServerBase();

	QString GetErrorString() const;
	void SetModeTest(bool mode_test);
	void SetToken(const std::string &token);
	virtual bool Run(quint16 Port);

protected:
	void SetErrorString(const QString &error_string);
	void Send(QTcpSocket *TcpSocket, const QVariantMap &Data); //Отправка данных
	void SendError(QTcpSocket *TcpSocket, const QString &ErrorString); //Отправка ошибки
	bool IsModeTest() const;
	std::vector<unsigned char>& GetToken();
	QString GetTokenString() const;

private:
	void AcceptError(QTcpSocket::SocketError);

private:
	QString ErrorString;
	bool ModeTest;
	std::vector<unsigned char> Token;
};
//-----------------------------------------------------------------------------
#endif
