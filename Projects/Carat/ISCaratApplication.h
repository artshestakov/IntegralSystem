#pragma once
#ifndef _ISCARATAPPLICATION_H_INCLUDED
#define _ISCARATAPPLICATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpServer.h"
#include "ISAsterisk.h"
//-----------------------------------------------------------------------------
void QtLog(QtMsgType MessageType, const QMessageLogContext &MessageLogContext, const QString &Message);
//-----------------------------------------------------------------------------
class ISCaratApplication : public QCoreApplication
{
	Q_OBJECT

signals:
    void Stopped();

public:
    ISCaratApplication(int &argc, char **argv);
	virtual ~ISCaratApplication();

	QString GetErrorString() const; //Получить текстовое описание ошибки
	bool Initialize(); //Инициализация
    bool Run(const QStringList &Arguments); //Запуск с параметрами
	bool Run(); //Стандартный запуск
	void Shutdown(); //Остановка приложения

private:
    void StopController();
    bool Help();
    bool Version();
    bool SendShutdown();
    bool ConfigCreate();
    bool ConfigReset();

private:
	QString ErrorString;
	ISTcpServer *TcpServer;
	ISAsterisk *Asterisk;

    QString PathFileStop;
};
//-----------------------------------------------------------------------------
#endif
