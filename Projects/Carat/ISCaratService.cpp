#include "ISCaratService.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISQuery.h"
#include "ISCountingTime.h"
#include "ISSystem.h"
#include "ISNetwork.h"
#include "ISDatabase.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
static QString QS_CARAT_CORE = PREPARE_QUERY("SELECT core_name, core_filename, core_localname "
											 "FROM _caratcore "
											 "WHERE NOT core_isdeleted "
											 "AND core_active "
											 "ORDER BY core_priority");
//-----------------------------------------------------------------------------
ISCaratService::ISCaratService(QObject *parent) : QObject(parent)
{
	EventLoop = new QEventLoop(this);
	connect(this, &ISCaratService::QuitLoop, EventLoop, &QEventLoop::quit);

	Uptime = QDateTime::currentDateTime();

	TcpServer = new QTcpServer(this);
	connect(TcpServer, &QTcpServer::newConnection, this, &ISCaratService::NewConnection);
}
//-----------------------------------------------------------------------------
ISCaratService::~ISCaratService()
{
	
}
//-----------------------------------------------------------------------------
void ISCaratService::StartService()
{
	ISQuery qSelectCore(QS_CARAT_CORE);
	qSelectCore.SetShowLongQuery(false);
	if (qSelectCore.Execute())
	{
		while (qSelectCore.Next()) //Обход ядер
		{
			QString Name = qSelectCore.ReadColumn("core_name").toString();
			QString LocalName = qSelectCore.ReadColumn("core_localname").toString();
			QString FileName = qSelectCore.ReadColumn("core_filename").toString() + (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows ? QString().append(".").append(EXTENSION_EXE) : QString());

			ISLOGGER_EMPTY();
			ISLOGGER_INFO("Core \"" + LocalName + "\": starting...");
			QString CorePath = ISDefines::Core::PATH_APPLICATION_DIR + '/' + FileName;

			ISProcessCore *ProcessCore = new ISProcessCore(Name, LocalName, CorePath, this);
			connect(ProcessCore, &ISProcessCore::readyReadStandardOutput, this, &ISCaratService::ReadyReadStandartOutput);
			Cores.append(ProcessCore);
			
			if (QFile::exists(CorePath)) //Если ядро существует
			{
				CoreStart(ProcessCore);
			}
			else //Ядро не существует
			{
				ISLOGGER_WARNING("Core \"" + LocalName + "\" not found. Path: " + CorePath);
				ISSystem::SleepSeconds(3);
			}
		}
	}

	if (Cores.count())
	{
		ISLOGGER_EMPTY();
	}
	else //Если активных ядер нет
	{
		ISLOGGER_INFO("Not found active cores");
	}

	if (!TcpServer->listen(QHostAddress::Any, CARAT_PORT)) //Если прослушивание порта не запущено
	{
		ISLOGGER_ERROR("Not listen port: " + QString::number(CARAT_PORT) + ". Error: " + TcpServer->errorString());
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::CoreStart(ISProcessCore *ProcessCore)
{
	ISCountingTime CountingTime;

	QTimer TimerTimeout;
	TimerTimeout.start(30000);
	connect(&TimerTimeout, &QTimer::timeout, [=] //Ожидание запуска ядра
	{
		if (!ProcessCore->GetRunning())
		{
			EventLoop->quit();
		}
	});

	//Запуск ядра
	QTimer::singleShot(100, ProcessCore, [ProcessCore] { ProcessCore->start(); });
	EventLoop->exec();
	TimerTimeout.stop();

	if (ProcessCore->GetRunning())
	{
		ISLOGGER_INFO("Core \"" + ProcessCore->GetLocalName() + "\" started: " + CountingTime.GetElapsed() + " msec, process " + ProcessCore->processId());
	}
	else
	{
		ISLOGGER_INFO("Not started core: " + ProcessCore->GetLocalName());
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::ReadyReadStandartOutput()
{
	ISProcessCore *ProcessCore = dynamic_cast<ISProcessCore*>(sender());
    QString CoreOutput;

    if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows)
    {
        QTextCodec *TextCodec = QTextCodec::codecForName(TEXT_CODEC_IBM866);
        CoreOutput = TextCodec->toUnicode(QString::fromLocal8Bit(ProcessCore->readAllStandardOutput()).toLocal8Bit());
    }
    else if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Linux)
    {
        CoreOutput = QString::fromLocal8Bit(ProcessCore->readAllStandardOutput());
    }

    QStringList StringList = CoreOutput.split("\n");
	for (QString &String : StringList) //Обход сообщений ядра
	{
		if (!String.length())
		{
			continue;
		}

		if (String.contains("\r"))
		{
			String.replace("\r", QString());
		}

		if (EventLoop->isRunning()) //Если ядро в процессе запуска
		{
			if (String == "exec") //Если ядро запустилось
			{
				ProcessCore->SetRunning(true);
				EventLoop->quit();
				break;
			}
		}
		ISLOGGER_UNKNOWN(String);
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::NewConnection()
{
	QTcpSocket *TcpSocket = TcpServer->nextPendingConnection();
	connect(TcpSocket, &QTcpSocket::disconnected, this, &ISCaratService::DisconnectedClient);
	connect(TcpSocket, &QTcpSocket::readyRead, this, &ISCaratService::ReadyRead);	
	VectorClients.append(TcpSocket);
}
//-----------------------------------------------------------------------------
void ISCaratService::DisconnectedClient()
{
	QTcpSocket *TcpSocket = dynamic_cast<QTcpSocket*>(sender());
	int SocketDescriptor = TcpSocket->socketDescriptor();

	for (int i = 0; i < VectorClients.count(); ++i)
	{
		if (VectorClients.at(i) == TcpSocket)
		{
			VectorClients.removeAt(i);

			if (Buffer.contains(SocketDescriptor))
			{
				Buffer.remove(SocketDescriptor);
			}
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::AppendSocketMessage(const QString &Message)
{
	for (QTcpSocket *TcpSocket : VectorClients)
	{
		WriteSocket(TcpSocket, Message);
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::ReadyRead()
{
	QTcpSocket *TcpSocket = dynamic_cast<QTcpSocket*>(sender());
	int SocketDescriptor = TcpSocket->socketDescriptor();
	QString String = TcpSocket->readAll();

	if (String == "\r\n") //Запрос новой строки от клиента
	{
		return;
	}

	if (!Buffer.contains(SocketDescriptor))
	{
		Buffer.insert(SocketDescriptor, String);
	}
	else
	{
		Buffer[SocketDescriptor].append(String);
	}

	if (String.contains("\r\n"))
	{
		String = Buffer.take(SocketDescriptor);
		String.replace("\r\n", QString());
		
		QStringList StringList = String.split(SYMBOL_SPACE);
		QString CompleteCommand;

		for (const QString &String : StringList)
		{
			QString Temp = String.toLower();
			Temp.replace(0, 1, Temp[0].toUpper());
			CompleteCommand.append(Temp);
		}

		if (ISSystem::CheckExistSlot(this, CompleteCommand))
		{
			QMetaObject::invokeMethod(this, CompleteCommand.toStdString().c_str(), Q_ARG(QTcpSocket *, TcpSocket));
		}
		else
		{
			
		}
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::WriteSocket(QTcpSocket *TcpSocket, const QString &String)
{
	if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows)
	{
		QTextStream TextStream(TcpSocket);
		TextStream.setCodec(TEXT_CODEC_IBM866);
		TextStream << String << "\r\n";
	}
	else if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Linux)
	{
        TcpSocket->write(QString::fromLocal8Bit(String.toStdString().c_str()).toUtf8() + "\r\n");
	}
}
//-----------------------------------------------------------------------------
