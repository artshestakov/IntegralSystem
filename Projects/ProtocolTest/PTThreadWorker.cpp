#include "PTThreadWorker.h"
#include "ISDatabase.h"
#include "ISTcpServerWorker.h"
//-----------------------------------------------------------------------------
PTThreadWorker::PTThreadWorker(quint16 port, const QString &token, const QString &login, const QString &password, QObject *parent)
	: QThread(parent),
	Port(port),
	Token(token),
	Login(login),
	Password(password)
{

}
//-----------------------------------------------------------------------------
PTThreadWorker::~PTThreadWorker()
{

}
//-----------------------------------------------------------------------------
QString PTThreadWorker::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void PTThreadWorker::Start(quint16 port)
{
	Port = port;
	start();
}
//-----------------------------------------------------------------------------
void PTThreadWorker::run()
{
	bool Result = ISDatabase::Instance().Connect("TestingConnection", "127.0.0.1", 5432, "oilsphere_db", Login, Password);
	if (!Result)
	{
		ErrorString = ISDatabase::Instance().GetErrorString();
	}

	ISTcpServerWorker ServerWorker(Token.toStdString());
	ServerWorker.SetModeTest(true);
	connect(&ServerWorker, &ISTcpServerWorker::DisconnectedClient, this, &PTThreadWorker::quit);
	if (Result)
	{
		Result = ServerWorker.Run(Port);
		if (!Result)
		{
			ErrorString = ServerWorker.GetErrorString();
		}
	}
	emit StateChanged(Result);
	exec();
	ISDatabase::Instance().Disconnect("TestingConnection");
}
//-----------------------------------------------------------------------------
