#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);

	quint16 Port = CARAT_DEFAULT_PORT;
	if (ISTcpConnector::Instance().Connect("127.0.0.1", Port))
	{
		ISTcpQuery qAuth(API_AUTH);
		qAuth.BindValue("Login", "shestakov");
		qAuth.BindValue("Password", "454Trendy");
		if (qAuth.Execute())
		{
			Port = qAuth.GetAnswer()["Port"].toInt();
		}
		else
		{
			qDebug() << qAuth.GetErrorString();
		}
		ISTcpConnector::Instance().Disconnect();
	}
	else
	{
		qDebug() << ISTcpConnector::Instance().GetErrorString();
	}

	if (ISTcpConnector::Instance().Connect("127.0.0.1", Port))
	{
		ISTcpQuery qTestQuery(API_TEST_QUERY);
		if (qTestQuery.Execute())
		{
			qDebug() << qTestQuery.GetAnswer()["DateTime"].toString();
		}
	}

	return Application.exec();
}
//-----------------------------------------------------------------------------
