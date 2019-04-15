#include "StdAfx.h"
#include "ISCaratService.h"
#include "EXDefines.h"
#include "EXConstants.h"
#include "ISDebug.h"
#include "ISQuery.h"
#include "ISCountingTime.h"
#include "ISLocalization.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISQueryPool.h"
#include "ISLicense.h"
#include "ISNetwork.h"
#include "ISDatabase.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
static QString QS_CARAT_CORE = PREPARE_QUERY("SELECT core_name, core_filename, core_localname "
											 "FROM _caratcore "
											 "WHERE NOT core_isdeleted "
											 "AND core_uid IN(SELECT crca_core FROM _caratcoreactive WHERE crca_active) "
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
		while (qSelectCore.Next()) //����� ����
		{
			QString Name = qSelectCore.ReadColumn("core_name").toString();
			QString LocalName = qSelectCore.ReadColumn("core_localname").toString();
			QString FileName = qSelectCore.ReadColumn("core_filename").toString();

			if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows)
			{
				 FileName += ".exe";
			}

			ISDebug::ShowEmptyString();
			ISDebug::ShowInfoString(LOCALIZATION("Core.Starting").arg(LocalName));
			QString CorePath = APPLICATION_DIR_PATH + "/" + FileName;

			ISProcessCore *ProcessCore = new ISProcessCore(Name, LocalName, CorePath, this);
			connect(ProcessCore, &ISProcessCore::readyReadStandardOutput, this, &ISCaratService::ReadyReadStandartOutput);
			connect(ProcessCore, &ISProcessCore::Crash, this, &ISCaratService::CrashedCore);
			Cores.append(ProcessCore);
			
			if (QFile::exists(CorePath)) //���� ���� ����������
			{
				CoreStart(ProcessCore);
			}
			else //���� �� ����������
			{
				ISDebug::ShowWarningString(LOCALIZATION("Core.NotFound").arg(LocalName).arg(CorePath));
				ISSystem::SleepSeconds(3);
				ISDebug::ShowInfoString(LOCALIZATION("Core.NotStarted").arg(LocalName));
			}
		}
	}

	if (Cores.count())
	{
		ISDebug::ShowEmptyString();
	}
	else //���� �������� ���� ���
	{
		ISDebug::ShowInfoString(LOCALIZATION("NotFoundActiveCores"));
	}

	if (TcpServer->listen(QHostAddress::Any, CARAT_PORT)) //���� ������������� ����� �������� �������
	{
		ISDebug::ShowInfoString(LOCALIZATION("Started").arg(CARAT_PORT));
		connect(&ISDebug::GetInstance(), &ISDebug::Output, this, &ISCaratService::AppendSocketMessage);
	}
	else //������������� ����� �� ��������
	{
		ISDebug::ShowCriticalString(LOCALIZATION("NotStarted").arg(CARAT_PORT).arg(TcpServer->errorString()));
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::CoreStart(ISProcessCore *ProcessCore)
{
	ISCountingTime CountingTime;

	QTimer TimerTimeout;
	TimerTimeout.start(30000);
	connect(&TimerTimeout, &QTimer::timeout, [=] //�������� ������� ����
	{
		if (!ProcessCore->GetRunning())
		{
			EventLoop->quit();
		}
	});

	//������ ����
	QTimer::singleShot(100, ProcessCore, [ProcessCore] { ProcessCore->start(); });
	EventLoop->exec();
	TimerTimeout.stop();

	if (ProcessCore->GetRunning())
	{
		ISDebug::ShowInfoString(LOCALIZATION("Core.Started").arg(ProcessCore->GetLocalName()).arg(CountingTime.GetElapsed()).arg(ProcessCore->processId()));
	}
	else
	{
		ISDebug::ShowInfoString(LOCALIZATION("Core.NotStarted").arg(ProcessCore->GetLocalName()));
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
	for (QString &String : StringList) //����� ��������� ����
	{
		if (!String.length())
		{
			continue;
		}

		if (String.contains("\r"))
		{
			String.replace("\r", QString());
		}

		if (EventLoop->isRunning()) //���� ���� � �������� �������
		{
			if (String == "exec") //���� ���� �����������
			{
				ProcessCore->SetRunning(true);
				EventLoop->quit();
				break;
			}
		}
		
		QString NewString = QString(String).replace(QRegExp(REG_EXP_DEBUG), QString());
		ISDebug::ShowCaratString(ProcessCore->GetName(), NewString);
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::CrashedCore(const QString &CoreName)
{
	if (SETTING_DATABASE_VALUE_DB(CONST_SETTING_DATABASE_CARAT_SEND_CRASH_REPORT).toBool()) //���� �������� ������ �������� � ���������� ���� ������
	{
		QString InformerPath = APPLICATION_DIR_PATH + "/Informer";
		if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows)
		{
			InformerPath.append(".exe");
		}

		QStringList Arguments;
		Arguments.append(CoreName);
		Arguments.append(ISLicense::GetInstance().GetName());

		ISDebug::ShowInfoString("Sending crash report...");
		QProcess::execute(InformerPath, Arguments);
		ISDebug::ShowInfoString("Crash report sended");
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::NewConnection()
{
	QTcpSocket *TcpSocket = TcpServer->nextPendingConnection();
	connect(TcpSocket, &QTcpSocket::disconnected, this, &ISCaratService::DisconnectedClient);
	connect(TcpSocket, &QTcpSocket::readyRead, this, &ISCaratService::ReadyRead);	
	
	ISDebug::ShowString(LOCALIZATION("Client.Connected").arg(ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString())).arg(VectorClients.count() + 1));
	WriteSocket(TcpSocket, LOCALIZATION("Greeting"));
	VectorClients.append(TcpSocket);
}
//-----------------------------------------------------------------------------
void ISCaratService::DisconnectedClient()
{
	QTcpSocket *TcpSocket = dynamic_cast<QTcpSocket*>(sender());
	int SocketDescriptor = TcpSocket->socketDescriptor();

	for (int i = 0; i < VectorClients.count(); i++)
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

	ISDebug::ShowString(LOCALIZATION("Client.Disconnected").arg(ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString())).arg(VectorClients.count()));
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

	if (String == "\r\n") //������ ����� ������ �� �������
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
			WriteSocket(TcpSocket, LOCALIZATION("CommandNotFound").arg(String));
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
void ISCaratService::Help(QTcpSocket *TcpSocket)
{
    Q_UNUSED(TcpSocket);
}
//-----------------------------------------------------------------------------
void ISCaratService::Exit(QTcpSocket *TcpSocket)
{
	TcpSocket->close();
}
//-----------------------------------------------------------------------------
void ISCaratService::Quit(QTcpSocket *TcpSocket)
{
	TcpSocket->close();
}
//-----------------------------------------------------------------------------
void ISCaratService::Close(QTcpSocket *TcpSocket)
{
	TcpSocket->close();
}
//-----------------------------------------------------------------------------
void ISCaratService::Disconnect(QTcpSocket *TcpSocket)
{
	TcpSocket->close();
}
//-----------------------------------------------------------------------------
void ISCaratService::Status(QTcpSocket *TcpSocket)
{
	WriteSocket(TcpSocket, LOCALIZATION("StartedDateTime").arg(ISDatabase::GetInstance().GetAge(Uptime)));
	for (ISProcessCore *Core : Cores) //������ ����
	{
		QString StateString;
		ISProcessCore::ProcessState CoreState = Core->state();
		if (CoreState == ISProcessCore::NotRunning)
		{
			StateString = Core->GetName() + ":\t" + LOCALIZATION("Process.ProcessState.NotRunning");
		}
		else if (CoreState == ISProcessCore::Starting)
		{
			StateString = Core->GetName() + ":\t" + LOCALIZATION("Process.ProcessState.Starting");
		}
		else if (CoreState == ISProcessCore::Running)
		{
			StateString = Core->GetName() + ":\t" + LOCALIZATION("Process.ProcessState.Running");
		}

		StateString.insert(0, "\r\n");
		WriteSocket(TcpSocket, StateString);
	}
}
//-----------------------------------------------------------------------------
