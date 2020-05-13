#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);

	QString Host = argv[1];
	quint16 Port = CARAT_DEFAULT_PORT;
	if (ISTcpConnector::Instance().Connect(Host, Port))
	{
		ISTcpQuery qAuth(API_AUTH);
		qAuth.BindValue("Login", "postgres");
		qAuth.BindValue("Password", "adm777");
		if (qAuth.Execute())
		{
			Port = qAuth.GetAnswer()["Port"].toInt();
		}
		else
		{
			qDebug() << qAuth.GetErrorString();
		}

		if (ISTcpConnector::Instance().Reconnect(Host, Port))
		{
			ISTcpQuery qTestQuery(API_TEST_QUERY);
			if (qTestQuery.Execute())
			{
				qDebug() << qTestQuery.GetAnswer()["DateTime"].toString();
			}
			ISTcpConnector::Instance().Disconnect();
		}
	}
	else
	{
		qDebug() << ISTcpConnector::Instance().GetErrorString();
	}
}
//-----------------------------------------------------------------------------
