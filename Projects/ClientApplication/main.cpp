#include "StdAfx.h"
#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);
	if (ISTcpConnector::Instance().Connect("127.0.0.1", 23071))
	{
		ISTcpQuery TcpQuery("Auth");
		TcpQuery.BindValue("Login", "postgres");
		TcpQuery.BindValue("Password", "adm777");
		if (TcpQuery.Execute())
		{

		}
		else
		{

		}
		ISTcpConnector::Instance().Disconnect();
	}
	else
	{
		qDebug() << ISTcpConnector::Instance().GetErrorString();
	}
	return Application.exec();
}
//-----------------------------------------------------------------------------
