#include "StdAfx.h"
#include "ISEMail.h"
#include "EXDefines.h"
#include "ISSystem.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
ISEMail::ISEMail(const QString &server, int port, const ISUuid &connection_type, const QString &sender_login, const QString &sender_password, const QString &sender_name, const QString &recipient_login, const QString &subject, const QString &message, QObject *parent)
	: QObject(parent),
	Server(server),
	Port(port),
	ConnectionType(connection_type),
	SenderLogin(sender_login),
	SenderPassword(sender_password),
	SenderName(sender_name),
	RecipientLogin(recipient_login),
	Subject(subject),
	Message(message)
{
	Process = new QProcess(this);
	Process->setProgram(APPLICATION_DIR_PATH + "/EMailSender");
	Process->setReadChannel(QProcess::StandardOutput);
	connect(Process, &QProcess::readyReadStandardOutput, this, &ISEMail::ReadyReadStandardOutput);
	connect(Process, &QProcess::readyReadStandardError, this, &ISEMail::ReadyReadStandardError);
}
//-----------------------------------------------------------------------------
ISEMail::ISEMail(QObject *parent) : ISEMail(QString(), -1, ISUuid(), QString(), QString(), QString(), QString(), QString(), QString())
{

}
//-----------------------------------------------------------------------------
ISEMail::~ISEMail()
{

}
//-----------------------------------------------------------------------------
void ISEMail::SetServer(const QString &server)
{
	Server = server;
}
//-----------------------------------------------------------------------------
void ISEMail::SetPort(int port)
{
	Port = port;
}
//-----------------------------------------------------------------------------
void ISEMail::SetConnectionType(const ISUuid &connection_type)
{
	ConnectionType = connection_type;
}
//-----------------------------------------------------------------------------
void ISEMail::SetSenderLogin(const QString &sender_login)
{
	SenderLogin = sender_login;
}
//-----------------------------------------------------------------------------
void ISEMail::SetSenderPassword(const QString &sender_password)
{
	SenderPassword = sender_password;
}
//-----------------------------------------------------------------------------
void ISEMail::SetSenderName(const QString &sender_name)
{
	SenderName = sender_name;
}
//-----------------------------------------------------------------------------
void ISEMail::SetRecipientLogin(const QString &recipient_login)
{
	RecipientLogin = recipient_login;
}
//-----------------------------------------------------------------------------
void ISEMail::SetSubject(const QString &subject)
{
	Subject = subject;
}
//-----------------------------------------------------------------------------
void ISEMail::SetMessage(const QString &message)
{
	Message = message;
}
//-----------------------------------------------------------------------------
QString ISEMail::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISEMail::SendMessage()
{
	bool Result = true;
	QString FilePath; //Путь к файлу отправки

	if (CreateSenderFile(FilePath)) //Если файл отправки открылся
	{
		Process->setArguments(QStringList() << FilePath);
		Process->start(); //Запуск утилиты отправки
		Result = Process->waitForFinished(5000); //Ожидание завершения работы утилиты
		if (!Result)
		{
			ErrorString = Process->errorString();
		}

		//Если работа утилиты завершилась с ошибкой (код возврата отличен от нуля) или утилита упала
		if (Process->exitCode() != 0 || Process->exitStatus() != QProcess::NormalExit)
		{
			Result = false;
		}

		QFile::remove(FilePath);
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISEMail::ReadyReadStandardOutput()
{
	QString Output = Process->readAllStandardOutput();
	QStringList StringList = Output.split("\r\n");
	StringList.removeOne(QString());

	for (const QString &String : StringList)
	{
		ISDebug::ShowInfoString(String);
	}
}
//-----------------------------------------------------------------------------
void ISEMail::ReadyReadStandardError()
{
	ErrorString = Process->readAllStandardError();
}
//-----------------------------------------------------------------------------
bool ISEMail::CreateSenderFile(QString &FilePath)
{
	QFile File(APPLICATION_TEMP_PATH + "/" + ISSystem::GenerateUuid() + ".json");
	if (File.open(QIODevice::WriteOnly)) //Если файл отправки открылся
	{
		//Запись контента и закрытие файла
		File.write(CreateJsonContent().toUtf8());
		File.close();
	}

	FilePath = File.fileName();
	return true;
}
//-----------------------------------------------------------------------------
QString ISEMail::CreateJsonContent() const
{
	QVariantMap VariantMap;
	VariantMap.insert("Server", Server);
	VariantMap.insert("Port", Port);
	VariantMap.insert("ConnectionType", ConnectionType);
	VariantMap.insert("SenderLogin", SenderLogin);
	VariantMap.insert("SenderPassword", SenderPassword);
	VariantMap.insert("SenderName", SenderName);
	VariantMap.insert("RecipientLogin", RecipientLogin);
	VariantMap.insert("Subject", Subject);
	VariantMap.insert("Message", Message);
	return ISSystem::VariantMapToJsonString(VariantMap);
}
//-----------------------------------------------------------------------------
