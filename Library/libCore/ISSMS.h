#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISSMS : public QObject
{
	Q_OBJECT

public:
	ISSMS(const QString &login, const QString &password, const QString &phone, const QString &message, const QString &charset, QObject *parent = 0);
	virtual ~ISSMS();

	void SetLogin(const QString &login);
	void SetPassword(const QString &password);
	void SetPhone(const QString &phone);
	void SetMessage(const QString &message);
	void SetCharset(const QString &charset);

	QString GetErrorString() const; //�������� ����� ������
	ISNamespace::SMSErrorType GetErrorType() const; //�������� ��� ������
	int GetMessageID() const; //�������� ������������� ���������
	bool SendMessage();

protected:
	QString CreateUrl() const;

private:
	QString ErrorString;
	ISNamespace::SMSErrorType ErrorType;

	QString Login;
	QString Password;
	QString Phone;
	QString Message;
	QString Charset;

	int MessageID;
};
//-----------------------------------------------------------------------------
