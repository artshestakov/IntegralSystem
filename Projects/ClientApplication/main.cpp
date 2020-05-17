#include "ISTcpConnector.h"
#include "ISTcpQueryAuth.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);

	QString Login = "postgres";
	QString Password = "adm777";

	QString Host = argv[1];
	quint16 Port = CARAT_DEFAULT_PORT;
	if (ISTcpConnector::Instance().Connect(Host, Port, Login, Password))
	{
		ISTcpQueryAuth qAuth(Login, Password);
		if (qAuth.Execute())
		{
			Port = qAuth.GetAnswer()["Port"].toInt();
		}
		else
		{
			qDebug() << qAuth.GetErrorString();
		}

		if (ISTcpConnector::Instance().Reconnect(Host, Port, Login, Password))
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
