#include "StdAfx.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISSmtpClient.h"
#include "ISEmailAddress.h"
#include "ISMimeMessage.h"
#include "ISMimeHtml.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication EMailSender(argc, argv);
	QTextStream StreamError(stderr);
	QTextStream StreamOutput(stdout);
	QStringList Arguments = EMailSender.arguments();

	if (Arguments.count() != 2) //Если аргумент не введен
	{
		StreamError << "Invalid arguments! Using: EMailSender SenderFile.json" << endl;
		return EXIT_CODE_ERROR;
	}

	QFile File(Arguments.last());
	if (!File.exists()) //Если файл не существует
	{
		StreamError << "File \"" << File.fileName() << "\" not exist" << endl;
		return EXIT_CODE_NORMAL;
	}

	if (!File.open(QIODevice::ReadOnly)) //Если файл не открылся
	{
		StreamError << "File \"" << File.fileName() << "\" not open:" << File.errorString() << endl;
		return EXIT_CODE_NORMAL;
	}

	QString FileContent = File.readAll();
	File.close();

	QStringMap StringMap = ISSystem::JsonStringToStringMap(FileContent);
	ISSmtpClient::ConnectionType ConnectionType;
	if (StringMap.value("ConnectionType") == CONST_UID_EMAIL_CONNECTION_TCP)
	{
		ConnectionType = ISSmtpClient::TcpConnection;
	}
	else if (StringMap.value("ConnectionType") == CONST_UID_EMAIL_CONNECTION_SSL)
	{
		ConnectionType = ISSmtpClient::SslConnection;
	}
	else if (StringMap.value("ConnectionType") == CONST_UID_EMAIL_CONNECTION_TLS)
	{
		ConnectionType = ISSmtpClient::TlsConnection;
	}
	else
	{
		ConnectionType = ISSmtpClient::TcpConnection;
	}

	ISSmtpClient SmtpClient(StringMap.value("Server"), StringMap.value("Port").toInt(), ConnectionType);
	ISEmailAddress EmailSender(StringMap.value("SenderLogin"), StringMap.value("SenderName")); //Отправитель
	ISEmailAddress EMailRecipient(StringMap.value("RecipientLogin")); //Получатель

	ISMimeMessage MimeMessage;
	MimeMessage.setSender(&EmailSender);
	MimeMessage.addRecipient(&EMailRecipient);
	MimeMessage.setSubject(StringMap.value("Subject")); //Тема сообщения

	ISMimeHtml MimeHtml;
	MimeHtml.setHtml(StringMap.value("Message")); //Сообщение
	MimeMessage.addPart(&MimeHtml);

	int ExitCode = EXIT_CODE_NORMAL;
	StreamOutput << "Connection to host..." << endl;
	if (SmtpClient.connectToHost())
	{
		StreamOutput << "Connect done" << endl;
		StreamOutput << "Authorization..." << endl;
		if (SmtpClient.login(StringMap.value("SenderLogin"), StringMap.value("SenderPassword")))
		{
			StreamOutput << "Authorization done" << endl;
			StreamOutput << "Sending..." << endl;
			if (SmtpClient.sendMail(MimeMessage))
			{
				StreamOutput << "Sending done" << endl;
			}
			else
			{
				StreamError << "Sending failed:" << SmtpClient.getErrorString() << endl;
				ExitCode = EXIT_CODE_ERROR;
			}
		}
		else
		{
			StreamError << "Authorization failed:" << SmtpClient.getErrorString() << endl;
			ExitCode = EXIT_CODE_ERROR;
		}

		SmtpClient.quit();
	}
	else
	{
		StreamError << "Connection failed:" << SmtpClient.getErrorString() << endl;
		ExitCode = EXIT_CODE_ERROR;
	}
	
	return ExitCode;
}
//-----------------------------------------------------------------------------
