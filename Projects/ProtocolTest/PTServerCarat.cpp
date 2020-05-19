#include "PTServerCarat.h"
#include "PTThreadWorker.h"
//-----------------------------------------------------------------------------
PTServerCarat::PTServerCarat(QObject *parent) : ISTcpServerCarat(parent)
{

}
//-----------------------------------------------------------------------------
PTServerCarat::~PTServerCarat()
{

}
//-----------------------------------------------------------------------------
bool PTServerCarat::StartWorker(QTcpSocket *TcpSocket, const QString &Port, const QString &Login, const QString &Password, const QString &Key)
{
	PTThreadWorker *ThreadWorker = new PTThreadWorker(Port.toInt(), Key, Login, Password);
	ThreadWorker->start();
	return true;
}
//-----------------------------------------------------------------------------
