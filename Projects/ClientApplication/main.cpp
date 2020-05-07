#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);
	if (ISTcpConnector::Instance().Connect("127.0.0.1", CARAT_DEFAULT_PORT))
	{
		ISTcpQuery TcpQuery(API_AUTH);
		TcpQuery.BindValue("Login", "shestakov");
		TcpQuery.BindValue("Password", "454Trendy");
		if (TcpQuery.Execute())
		{
			qDebug() << "Ok";
		}
		else
		{
			qDebug() << TcpQuery.GetErrorString();
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
