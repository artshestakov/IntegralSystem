#include "ISCaratController.h"
#include "ISConstants.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
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

	//��������� ��������� � ���������� ����
	ControllerPort = CONFIG_INT(CONST_CONFIG_CONTROLLER_PORT);
	if (ControllerPort < 1 || ControllerPort >= USHRT_MAX) //���� �������� �� ������ � �������� ������ - ���������� ���� �� ���������
	{
		ISLOGGER_W(__CLASS__, QString("Invalid config value %1: %2. The default port will be used - %3.").arg(CONST_CONFIG_CONTROLLER_PORT).arg(ControllerPort).arg(CARAT_CONTROLLER_PORT));
		ControllerPort = CARAT_CONTROLLER_PORT;
	}

	QEventLoop EventLoop;
	connect(this, &ISCaratController::Started, &EventLoop, &QEventLoop::quit);
	start();
	EventLoop.exec();

	IsRunning ? ISLOGGER_I(__CLASS__, "Started") : ISLOGGER_E(__CLASS__, "Not started: " + ErrorString);
	return IsRunning;
}
//-----------------------------------------------------------------------------
void ISCaratController::Stop()
{
	//������������� ����������
	ISLOGGER_I(__CLASS__, "Stopped");
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);

	//��� ����� �� ����������� � ������� �� �������
	while (!IsFinished)
	{
		ISSleep(10);
	}
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
	
	while (true) //������ ������
	{
		//���������, �� ���������� �� �����
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_running = IsRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (!is_running) //���� ����� ������� ��� "�� ��������", �� ������� �� �����
		{
			break;
		}

		Wait(); //��� ���������� ������ ������� � ��������
		if (TcpServer.hasPendingConnections()) //���� ��������� ����� �����������
		{
			QTcpSocket *TcpSocket = TcpServer.nextPendingConnection(); //�������� ����� �����������
			while (true) //��� ���� �� ������ ������ �� �������
			{
				Wait(); //��� ���������� ������ ������� � ��������

				if (!TcpSocket->bytesAvailable()) //���� ������ �� ������ - ���������� �����
				{
					continue;
				}

				QByteArray Command = TcpSocket->readAll().toLower();
				if (Command == "shutdown") //������ ������� �� ���������� ������
				{
					ISLOGGER_I(__CLASS__, "Incoming command: " + Command);
					Send(TcpSocket, "OK\n");
					emit Shutdown();
				}
				else //������ ����������� �������
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
	ISSystem::ProcessEvents();
}
//-----------------------------------------------------------------------------
