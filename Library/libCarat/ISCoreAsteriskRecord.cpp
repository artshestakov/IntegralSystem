#include "ISCoreAsteriskRecord.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISDebug.h"
#include "ISSettingsDatabase.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISNetwork.h"
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
		ISDebug::ShowWarningString(LANG("AsteriskRecord.Server.RepositoryRecordsNotSetting"));
		return false;
	}

	if (QDir(RepositoryPath).exists()) //���� ����� � ���������� ������� ���������� �� ����������
	{
		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.RepositoryRecords").arg(RepositoryPath));
	}
	else
	{
		ISDebug::ShowWarningString(LANG("AsteriskRecord.Server.RepositoryRecordsNotFound").arg(RepositoryPath));
		return false;
	}

	int ListenPort = SETTING_DATABASE_VALUE_DB(CONST_UID_DATABASE_SETTING_ASTERISK_RECORDS_PORT).toInt();
	if (!ListenPort || ListenPort < 0)
	{
		ISDebug::ShowWarningString(LANG("AsteriskRecord.Server.RecordsPortInvalid").arg(ListenPort));
		return false;
	}

	QString Extension = SETTING_DATABASE_VALUE_DB(CONST_UID_DATABASE_SETTING_ASTERISK_RECORDS_EXTENSION).toString();
	if (!Extension.length())
	{
		ISDebug::ShowWarningString(LANG("AsteriskRecord.Server.RecordsExtensionNotSetting"));
		return false;
	}

	TcpServer = new QTcpServer(this);
	if (TcpServer->listen(QHostAddress::Any, ListenPort))
	{
		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.ListenPort.Done").arg(TcpServer->serverPort()));
		connect(TcpServer, &QTcpServer::newConnection, this, &ISCoreAsteriskRecord::NewConnection);
		Started();
	}
	else
	{
		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.ListenPort.Error").arg(TcpServer->serverPort()));
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
		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.Client.Connected").arg(ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString())));
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
		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.Client.Disconnected").arg(ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString())));
	}
}
//-----------------------------------------------------------------------------
void ISCoreAsteriskRecord::ReadyRead()
{
	QTcpSocket *TcpSocket = dynamic_cast<QTcpSocket*>(sender());
	if (TcpSocket)
	{
		QString FileName = TcpSocket->readAll();
		FileName.replace("\n", QString());

		QFile FileRecord(RepositoryPath + '/' + FileName + SYMBOL_POINT + EXTENSION_WAV);

		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.SearchFile").arg(FileRecord.fileName()));
		if (FileRecord.exists()) //������������� ���� ����������
		{
			ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.SearchFile.Complete"));
		}
		else //������������� ���� �� ����������
		{
			ISDebug::ShowWarningString(LANG("AsteriskRecord.Server.FileNotExist").arg(FileName).arg(RepositoryPath));
			TcpSocket->write("message:AsteriskRecord.Server.Message.FileNotExist");
			return;
		}

		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.FileOpening").arg(FileRecord.fileName()));
		if (FileRecord.open(QIODevice::ReadOnly)) //���� ������������� ���� �� �����������
		{
			ISDebug::ShowInfoString("AsteriskRecord.Server.FileOpening.Complete");
		}
		else
		{
			ISDebug::ShowWarningString(LANG("AsteriskRecord.Server.FileNotOpen").arg(FileName).arg(FileRecord.errorString()));
			TcpSocket->write("message:AsteriskRecord.Server.Message.FileNotOpen");
			return;
		}

		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.ReadFile"));
		QByteArray RecordData = FileRecord.readAll(); //������ ����� �������
		FileRecord.close(); //�������� �����

		int Size = 0; //����� ������ �����
		QVector<QByteArray> Vector;
		while (RecordData.length()) //���������� ������� ����� �� �����
		{
			QByteArray ByteArray = RecordData.mid(0, 1000 * 128);
			Size += ByteArray.size();
			Vector.append(ByteArray);
			RecordData.remove(0, 1000 * 128);
		}

		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.SendFile.Info"));
		TcpSocket->write(QString("start:%1").arg(Size).toUtf8()); //�������� ������� �����
		ISSystem::ExecLoop(500); //�������� ����� ��������� �����

		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.SendFile.Start"));
		int Sended = 0; //������ ������������ ����������
		while (Vector.count()) //�������� �����
		{
			QByteArray ByteArray = Vector.takeFirst(); //�������� �� ������� ������ ����
			if (TcpSocket->isOpen()) //���� ����� ������
			{
				TcpSocket->write(ByteArray);
				Sended += ByteArray.size();

				ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.SendFile.Process").arg(Sended / 1000).arg(Size / 1000));
				ISSystem::ExecLoop(10); //�������� � 50 msec
			}
			else //����� ��������
			{
				ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.SendFile.Stop"));
				Vector.clear();
				return;
			}
		}

		ISDebug::ShowInfoString(LANG("AsteriskRecord.Server.SendFile.End"));
		ISSystem::ExecLoop(500); //�������� ����� �������� �����
		TcpSocket->write(QString("end:" + FileName).toUtf8());
	}
}
//-----------------------------------------------------------------------------
