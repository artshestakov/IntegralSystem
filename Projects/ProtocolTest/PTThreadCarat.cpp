#include "PTThreadCarat.h"
#include "ISTcpServerCarat.h"
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
	ISTcpServerCarat ServerCarat;
	bool Result = ServerCarat.Run(Port);
	if (!Result)
	{
		ErrorString = ServerCarat.GetErrorString();
	}
	emit StateChanged(Result);
	exec();
}
//-----------------------------------------------------------------------------
