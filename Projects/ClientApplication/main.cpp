#include "StdAfx.h"
#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);
	if (ISTcpConnector::Instance().Connect("127.0.0.1", CARAT_DEFAULT_PORT))
	{
		ISTcpQuery TcpQuery("Auth1");
		TcpQuery.BindValue("Login", "postgres");
		TcpQuery.BindValue("Password", "adm777");
		if (TcpQuery.Execute())
		{
			qDebug() << "Ok";
		}
		else
		{
			qDebug() << TcpQuery.GetErrorString();
		}
		return Application.exec();
		ISTcpConnector::Instance().Disconnect();
	}
	else
	{
		qDebug() << ISTcpConnector::Instance().GetErrorString();
	}
	return Application.exec();
}
//-----------------------------------------------------------------------------
