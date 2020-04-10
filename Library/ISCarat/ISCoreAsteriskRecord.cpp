#include "ISCoreAsteriskRecord.h"
#include "ISSettingsDatabase.h"
#include "ISSystem.h"
#include "ISNetwork.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISCoreAsteriskRecord::ISCoreAsteriskRecord(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{
	
}
//-----------------------------------------------------------------------------
ISCoreAsteriskRecord::~ISCoreAsteriskRecord()
{

}
//-----------------------------------------------------------------------------
bool ISCoreAsteriskRecord::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (!Result)
	{
		return Result;
	}
	
	RepositoryPath = SETTING_DATABASE_VALUE_DB(CONST_UID_DATABASE_SETTING_ASTERISK_REPOSITORY_RECORDS).toString();
	if (!RepositoryPath.length()) //���� ����� � ���������� ������� ���������� �� ���������
	{
		ISLOGGER_WARNING("Directory with voice records not setting");
		return false;
	}

	if (!QDir(RepositoryPath).exists()) //���� ����� � ���������� ������� ���������� �� ����������
	{
		ISLOGGER_WARNING("Directory with voice record not exist: " + RepositoryPath);
		return false;
	}

	int ListenPort = SETTING_DATABASE_VALUE_DB(CONST_UID_DATABASE_SETTING_ASTERISK_RECORDS_PORT).toInt();
	if (!ListenPort || ListenPort < 0)
	{
		ISLOGGER_WARNING("Invalid port: " + QString::number(ListenPort));
		return false;
	}

	QString Extension = SETTING_DATABASE_VALUE_DB(CONST_UID_DATABASE_SETTING_ASTERISK_RECORDS_EXTENSION).toString();
	if (!Extension.length())
	{
		ISLOGGER_WARNING("Extension for voice record not setting");
		return false;
	}

	TcpServer = new QTcpServer(this);
	if (TcpServer->listen(QHostAddress::Any, ListenPort))
	{
		ISLOGGER_INFO("Listen port: " + QString::number(TcpServer->serverPort()));
		connect(TcpServer, &QTcpServer::newConnection, this, &ISCoreAsteriskRecord::NewConnection);
		Started();
	}
	else
	{
		ISLOGGER_INFO("Error listen port: " + QString::number(TcpServer->serverPort()));
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
void ISCoreAsteriskRecord::NewConnection()
{
	QTcpSocket *TcpSocket = TcpServer->nextPendingConnection();
	if (TcpSocket)
	{
		ISLOGGER_INFO("New connection client from: " + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
		connect(TcpSocket, &QTcpSocket::readyRead, this, &ISCoreAsteriskRecord::ReadyRead);
		connect(TcpSocket, &QTcpSocket::disconnected, this, &ISCoreAsteriskRecord::Disconnected);
	}
}
//-----------------------------------------------------------------------------
void ISCoreAsteriskRecord::Disconnected()
{
	QTcpSocket *TcpSocket = dynamic_cast<QTcpSocket*>(sender());
	if (TcpSocket)
	{
		TcpSocket->close();
		ISLOGGER_INFO("Disconnected client from: " + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
	}
}
//-----------------------------------------------------------------------------
void ISCoreAsteriskRecord::ReadyRead()
{
	QTcpSocket *TcpSocket = dynamic_cast<QTcpSocket*>(sender());
	if (!TcpSocket)
	{
		return;
	}

	QString FileName = TcpSocket->readAll();
	FileName.replace("\n", QString());

	QFile FileRecord(RepositoryPath + '/' + FileName + SYMBOL_POINT + EXTENSION_WAV);

	ISLOGGER_INFO("Search file: " + FileRecord.fileName());
	if (!FileRecord.exists()) //������������� ���� �� ����������
	{
		ISLOGGER_WARNING("File \"" + FileName + "\" not exist");
		TcpSocket->write("message:AsteriskRecord.Server.Message.FileNotExist");
		return;
	}

	if (!FileRecord.open(QIODevice::ReadOnly)) //���� ������������� ���� �� �����������
	{
		ISLOGGER_WARNING("Error open file \"" + FileName + "\": " + FileRecord.errorString());
		TcpSocket->write("message:AsteriskRecord.Server.Message.FileNotOpen");
		return;
	}

	QByteArray RecordData = FileRecord.readAll(); //������ ����� �������
	FileRecord.close(); //�������� �����

	int Size = 0; //����� ������ �����
	ISVectorByteArray Vector;
	while (!RecordData.isEmpty()) //���������� ������� ����� �� �����
	{
		QByteArray ByteArray = RecordData.mid(0, 1000 * 128);
		Size += ByteArray.size();
		Vector.emplace_back(ByteArray);
		RecordData.remove(0, 1000 * 128);
	}

	ISLOGGER_INFO("Sending file...");
	TcpSocket->write(QString("start:%1").arg(Size).toUtf8()); //�������� ������� �����
	ISSystem::ExecLoop(500); //�������� ����� ��������� �����

	int Sended = 0; //������ ������������ ����������
	while (!Vector.empty()) //�������� �����
	{
		QByteArray ByteArray = ISAlgorithm::VectorTakeFront(Vector); //�������� �� ������� ������ ����
		if (TcpSocket->isOpen()) //���� ����� ������
		{
			TcpSocket->write(ByteArray);
			Sended += ByteArray.size();
			ISLOGGER_INFO("Sended " + QString::number(Sended / 1000) + "kb of " + QString::number(Size / 1000) + "kb");
			ISSystem::ExecLoop(10); //�������� � 50 msec
		}
		else //����� ��������
		{
			ISLOGGER_INFO("Stop send file");
			return;
		}
	}

	ISLOGGER_INFO("Sending finish");
	ISSystem::ExecLoop(500); //�������� ����� �������� �����
	TcpSocket->write(QString("end:" + FileName).toUtf8());
}
//-----------------------------------------------------------------------------
