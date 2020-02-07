#include "ISCoreAsteriskRecord.h"
#include "ISDebug.h"
#include "ISSettingsDatabase.h"
#include "ISSystem.h"
#include "ISNetwork.h"
#include "ISConstants.h"
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
	if (!RepositoryPath.length()) //Если папка с хранилищем записей разговоров не настроена
	{
		ISDebug::ShowWarningString("Directory with voice records not setting");
		return false;
	}

	if (!QDir(RepositoryPath).exists()) //Если папка с хранилищем записей разговоров не существует
	{
		ISDebug::ShowWarningString("Directory with voice record not exist: " + RepositoryPath);
		return false;
	}

	int ListenPort = SETTING_DATABASE_VALUE_DB(CONST_UID_DATABASE_SETTING_ASTERISK_RECORDS_PORT).toInt();
	if (!ListenPort || ListenPort < 0)
	{
		ISDebug::ShowWarningString("Invalid port: " + QString::number(ListenPort));
		return false;
	}

	QString Extension = SETTING_DATABASE_VALUE_DB(CONST_UID_DATABASE_SETTING_ASTERISK_RECORDS_EXTENSION).toString();
	if (!Extension.length())
	{
		ISDebug::ShowWarningString("Extension for voice record not setting");
		return false;
	}

	TcpServer = new QTcpServer(this);
	if (TcpServer->listen(QHostAddress::Any, ListenPort))
	{
		ISDebug::ShowInfoString("Listen port: " + QString::number(TcpServer->serverPort()));
		connect(TcpServer, &QTcpServer::newConnection, this, &ISCoreAsteriskRecord::NewConnection);
		Started();
	}
	else
	{
		ISDebug::ShowInfoString("Error listen port: " + QString::number(TcpServer->serverPort()));
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
		ISDebug::ShowInfoString("New connection client from: " + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
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
		ISDebug::ShowInfoString("Disconnected client from: " + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
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

	ISDebug::ShowInfoString("Search file: " + FileRecord.fileName());
	if (!FileRecord.exists()) //Запрашиваемый файл не существует
	{
		ISDebug::ShowWarningString("File \"" + FileName + "\" not exist");
		TcpSocket->write("message:AsteriskRecord.Server.Message.FileNotExist");
		return;
	}

	if (!FileRecord.open(QIODevice::ReadOnly)) //Если запрашиваемый файл не открывается
	{
		ISDebug::ShowWarningString("Error open file \"" + FileName + "\": " + FileRecord.errorString());
		TcpSocket->write("message:AsteriskRecord.Server.Message.FileNotOpen");
		return;
	}

	QByteArray RecordData = FileRecord.readAll(); //Чтение файла целиком
	FileRecord.close(); //Закрытие файла

	int Size = 0; //Общий размер файла
	QVector<QByteArray> Vector;
	while (RecordData.length()) //Разделение сжатого файла на блоки
	{
		QByteArray ByteArray = RecordData.mid(0, 1000 * 128);
		Size += ByteArray.size();
		Vector.append(ByteArray);
		RecordData.remove(0, 1000 * 128);
	}

	ISDebug::ShowInfoString("Sending file...");
	TcpSocket->write(QString("start:%1").arg(Size).toUtf8()); //Отправка размера файла
	ISSystem::ExecLoop(500); //Задержка перед отправкой файла

	int Sended = 0; //Размер отправленной информации
	while (Vector.count()) //Отправка файла
	{
		QByteArray ByteArray = Vector.takeFirst(); //Забираем из вектора первый блок
		if (TcpSocket->isOpen()) //Если сокет открыт
		{
			TcpSocket->write(ByteArray);
			Sended += ByteArray.size();
			ISDebug::ShowInfoString("Sended " + QString::number(Sended / 1000) + "kb of " + QString::number(Size / 1000) + "kb");
			ISSystem::ExecLoop(10); //Задержка в 50 msec
		}
		else //Сокет закрылся
		{
			ISDebug::ShowInfoString("Stop send file");
			return;
		}
	}

	ISDebug::ShowInfoString("Sending finish");
	ISSystem::ExecLoop(500); //Задержка после отправки файла
	TcpSocket->write(QString("end:" + FileName).toUtf8());
}
//-----------------------------------------------------------------------------
