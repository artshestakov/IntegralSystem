#include "ISCaratController.h"
#include "ISConstants.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISCaratController::ISCaratController(QObject *parent)
	: QThread(parent),
	ErrorString(NO_ERROR_STRING),
	ControllerPort(0),
	IsRunning(false),
	IsFinished(false)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISCaratController::~ISCaratController()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
QString ISCaratController::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISCaratController::Start()
{
	ISLOGGER_I(__CLASS__, "Starting...");

	//Получаем порт из настроек
	ControllerPort = CONFIG_INT(CONST_CONFIG_CONTROLLER_PORT);

	QEventLoop EventLoop;
	connect(this, &ISCaratController::Started, &EventLoop, &QEventLoop::quit);
	start();
	EventLoop.exec();

	IsRunning ?
		ISLOGGER_I(__CLASS__, QString("Started. Port %1").arg(ControllerPort)) :
		ISLOGGER_E(__CLASS__, QString("Not started with port %1: %2").arg(ControllerPort).arg(ErrorString));
	return IsRunning;
}
//-----------------------------------------------------------------------------
void ISCaratController::Stop()
{
	//Останавливаем контроллер
	ISLOGGER_I(__CLASS__, "Stopping");
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);

	//Ждём когда он остановится и выходим из функции
	while (!IsFinished)
	{
		ISSleep(10);
	}
	ISLOGGER_I(__CLASS__, "Stopped");
}
//-----------------------------------------------------------------------------
void ISCaratController::run()
{
	QTcpServer TcpServer;
	TcpServer.setMaxPendingConnections(1);
	connect(&TcpServer, &QTcpServer::acceptError, [&TcpServer]
	{
		ISLOGGER_E(__CLASS__, TcpServer.errorString());
	});
	IsRunning = TcpServer.listen(QHostAddress::LocalHost, ControllerPort);
	emit Started();

	if (!IsRunning)
	{
		ErrorString = TcpServer.errorString();
	}
	
	while (true) //Работа потока
	{
		//Проверяем, не остановили ли поток
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_running = IsRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (!is_running) //Если поток помечен как "не активный", то выходим из цикла
		{
			break;
		}

		Wait(); //Даём поработать другим потокам и событиям
		if (TcpServer.hasPendingConnections()) //Если появилось новое подключение
		{
			QTcpSocket *TcpSocket = TcpServer.nextPendingConnection(); //Доступно новое подключение
			while (true) //Ждём пока не придут данные от клиента
			{
				Wait(); //Даём поработать другим потокам и событиям

				if (!TcpSocket->bytesAvailable()) //Если данные не пришли - продолжаем ждать
				{
					continue;
				}

				QByteArray Command = TcpSocket->readAll().toLower();
				if (Command == CARAT_LOCAL_API_SHUTDOWN) //Пришла команда на завершение работы
				{
					ISLOGGER_I(__CLASS__, "Incoming command: " + Command);
					Send(TcpSocket, "OK\n");
					emit Shutdown();
				}
				else //Пришла неизвестная команда
				{
					QByteArray b("Command \"" + Command + "\" not support");
					ISLOGGER_W(__CLASS__, b);
					Send(TcpSocket, b);
				}
				delete TcpSocket;
				break;
			}
		}
	}
	TcpServer.close();
	IsFinished = true;
}
//-----------------------------------------------------------------------------
void ISCaratController::Send(QTcpSocket *TcpSocket, const QByteArray &Responce)
{
	TcpSocket->write(Responce);
	while (TcpSocket->bytesToWrite() > 0)
	{
		Wait();
	}
	ISSleep(10);
}
//-----------------------------------------------------------------------------
void ISCaratController::Wait()
{
	ISSleep(10);
	PROCESS_EVENTS();
}
//-----------------------------------------------------------------------------
