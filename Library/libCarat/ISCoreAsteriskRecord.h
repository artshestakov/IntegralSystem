#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBCARAT_EXPORT ISCoreAsteriskRecord : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreAsteriskRecord(int &argc, char **argv);
	virtual ~ISCoreAsteriskRecord();

	bool Invoke() override;

protected:
	void NewConnection(); //Новое подключение
	void Disconnected(); //Отключение
	void ReadyRead();

private:
	QString RepositoryPath;
	QTcpServer *TcpServer;
};
//-----------------------------------------------------------------------------
