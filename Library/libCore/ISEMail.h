#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISEMail : public QObject
{
	Q_OBJECT

public:
	ISEMail(const QString &server, int port, const ISUuid &connection_type, const QString &sender_login, const QString &sender_password, const QString &sender_name, const QString &recipient_login, const QString &subject, const QString &message, QObject *parent = 0);
	ISEMail(QObject *parent = 0);
	virtual ~ISEMail();

	void SetServer(const QString &server);
	void SetPort(int port);
	void SetConnectionType(const ISUuid &connection_type);
	void SetSenderLogin(const QString &sender_login);
	void SetSenderPassword(const QString &sender_password);
	void SetSenderName(const QString &sender_name);
	void SetRecipientLogin(const QString &recipient_login);
	void SetSubject(const QString &subject);
	void SetMessage(const QString &message);

	QString GetErrorString() const; //Получить текст ошибки
	bool SendMessage(); //Отправить сообщение

protected:
	void ReadyReadStandardOutput();
	void ReadyReadStandardError();
	bool CreateSenderFile(QString &FilePath); //Создание файла отправки
	QString CreateJsonContent() const; //Создание Json контента

private:
	QProcess *Process;
	QString ErrorString;

	QString Server;
	int Port;
	ISUuid ConnectionType;
	QString SenderLogin;
	QString SenderPassword;
	QString SenderName;
	QString RecipientLogin;
	QString Subject;
	QString Message;
};
//-----------------------------------------------------------------------------
