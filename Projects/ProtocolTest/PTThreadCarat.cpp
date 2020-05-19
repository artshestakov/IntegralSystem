#include "PTThreadCarat.h"
#include "PTServerCarat.h"
#include "ISDatabase.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
PTThreadCarat::PTThreadCarat(QObject *parent)
	: QThread(parent),
	Port(0)
{

}
//-----------------------------------------------------------------------------
PTThreadCarat::~PTThreadCarat()
{

}
//-----------------------------------------------------------------------------
QString PTThreadCarat::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void PTThreadCarat::Start(quint16 port)
{
	Port = port;
	start();
}
//-----------------------------------------------------------------------------
void PTThreadCarat::run()
{
	bool Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, "127.0.0.1", 5432, "oilsphere_db", "postgres", "adm777");
	if (!Result)
	{
		ErrorString = ISDatabase::Instance().GetErrorString();
	}

	PTServerCarat ServerCarat;
	ServerCarat.SetModeTest(true);
	if (Result)
	{
		Result = ServerCarat.Run(Port);
		if (Result)
		{
			ServerCarat.SetDBHost("127.0.0.1");
			ServerCarat.SetDBPort(5432);
			ServerCarat.SetDBName("oilsphere_db");
		}
		else
		{
			ErrorString = ServerCarat.GetErrorString();
		}
	}
	emit StateChanged(Result);
	exec();
	ISDatabase::Instance().Disconnect(CONNECTION_DEFAULT);
}
//-----------------------------------------------------------------------------
