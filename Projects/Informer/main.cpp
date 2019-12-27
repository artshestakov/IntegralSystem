#include "StdAfx.h"
#include "ISDefines.h"
#include "ISSmtpClient.h"
#include "ISEmailAddress.h"
#include "ISMimeMessage.h"
#include "ISMimeHtml.h"
#include "ISDebug.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication InformerApp(argc, argv);
	QStringList StringList = InformerApp.arguments();
	StringList.removeFirst();
	if (StringList.length() != 2)
	{
		ISDebug::ShowString("Invalid arguments! Using: Informer [CORE_NAME] [LICENSE_NAME]");
		return EXIT_CODE_ERROR;
	}

    QString CoreName = StringList.first();
    QString LicenseName = StringList.last();

	QFile File(APPLICATION_LOGS_PATH + "/Carat" + CoreName + "_" + QDateTime::currentDateTime().toString(DATE_FORMAT_V2) + "." + EXTENSION_LOG);
	if (!File.open(QIODevice::ReadOnly))
	{
		return EXIT_CODE_ERROR;
	}

	QString ContentLog = File.readAll();
	File.close();

	ISSmtpClient SmtpClient(CARAT_INFORMER_SERVER, CARAT_INFORMER_PORT, ISSmtpClient::SslConnection);
	ISEmailAddress EmailSender(CARAT_INFORMER_LOGIN, CARAT_INFORMER_NAME);
	ISEmailAddress EMailRecipient(CARAT_INFORMER_RECIPIENT);

	ISMimeMessage MimeMessage;
	MimeMessage.setSender(&EmailSender);
	MimeMessage.addRecipient(&EMailRecipient);
	MimeMessage.setSubject(LicenseName); //Тема сообщения

	ISMimeHtml MimeHtml;
	MimeHtml.setHtml(ContentLog);
	MimeMessage.addPart(&MimeHtml);

	ISDebug::ShowString("Connecting to host from send report...");
	if (SmtpClient.connectToHost())
	{
		ISDebug::ShowString(QString("Connected"));
		ISDebug::ShowString(QString("Check login and password..."));
		if (SmtpClient.login(CARAT_INFORMER_LOGIN, CARAT_INFORMER_PASSWORD))
		{
			ISDebug::ShowString("Checking");
			ISDebug::ShowString(QString("Sending report from: %1...").arg(CARAT_INFORMER_RECIPIENT));
			if (SmtpClient.sendMail(MimeMessage))
			{
				ISDebug::ShowString("Sended report done");
			}
			else
			{
				ISDebug::ShowString("Send failed");
			}
		}
		else
		{
			ISDebug::ShowString("Check failed");
		}
	}
	else
	{
		ISDebug::ShowString("Connected failed");
	}

	ISSystem::ExecLoop(1000);

	SmtpClient.quit();
}
//-----------------------------------------------------------------------------
