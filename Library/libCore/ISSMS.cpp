#include "ISSMS.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISSMS::ISSMS(const QString &login, const QString &password, const QString &phone, const QString &message, const QString &charset, QObject *parent) : QObject(parent)
{
	ErrorType = ISNamespace::SET_NoError;
	Login = login;
	Password = password;
	Phone = phone;
	Message = message;
	Charset = charset;
	MessageID = 0;
}
//-----------------------------------------------------------------------------
ISSMS::~ISSMS()
{

}
//-----------------------------------------------------------------------------
void ISSMS::SetLogin(const QString &login)
{
	Login = login;
}
//-----------------------------------------------------------------------------
void ISSMS::SetPassword(const QString &password)
{
	Password = password;
}
//-----------------------------------------------------------------------------
void ISSMS::SetPhone(const QString &phone)
{
	Phone = phone;
}
//-----------------------------------------------------------------------------
void ISSMS::SetMessage(const QString &message)
{
	Message = message;
}
//-----------------------------------------------------------------------------
void ISSMS::SetCharset(const QString &charset)
{
	Charset = charset;
}
//-----------------------------------------------------------------------------
QString ISSMS::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
ISNamespace::SMSErrorType ISSMS::GetErrorType() const
{
	return ErrorType;
}
//-----------------------------------------------------------------------------
int ISSMS::GetMessageID() const
{
	return MessageID;
}
//-----------------------------------------------------------------------------
bool ISSMS::SendMessage()
{
	bool Result = true;
	QEventLoop EventLoop;
	QNetworkAccessManager NetworkAccessManager;
	connect(&NetworkAccessManager, &QNetworkAccessManager::finished, &EventLoop, &QEventLoop::quit);

	QUrl Url(CreateUrl());
	QNetworkRequest NetworkRequest(Url);
	QNetworkReply *NetworkReply = NetworkAccessManager.get(NetworkRequest);
	EventLoop.exec();

	if (NetworkReply) //Если ответ от сервера пришёл
	{
		if (NetworkReply->error() == QNetworkReply::NoError) //Если ошибок в hhtps запрос нет
		{
			QByteArray ByteArray = NetworkReply->readAll();
			ISStringMap StringMap = ISSystem::JsonStringToStringMap(ByteArray);
			if (StringMap.count("error_code")) //При отправке произошка ошибка
			{
				ErrorString = StringMap["error"]; //Описание ошибки от сервиса
				ErrorType = qvariant_cast<ISNamespace::SMSErrorType>(StringMap["error_code"]); //Код ошибки сервиса
				Result = false;
			}

			if (StringMap.count("id"))
			{
				MessageID = StringMap["id"].toInt(); //Идентификатор отправленного сообщения
			}
		}
		else //Ошибка в отправке https запроса
		{
			ErrorString = NetworkReply->errorString();
			Result = false;
		}

		delete NetworkReply;
	}
	else
	{
		ErrorString = "Unknown error";
		Result = false;
	}

	return Result;
}
//-----------------------------------------------------------------------------
QString ISSMS::CreateUrl() const
{
	QString UrlString = "https://smsc.ru/sys/send.php?";
	QString UrlLogin = "login=" + Login;
	QString UrlPassword = "psw=" + Password;
	QString UrlPhone = "phones=" + Phone;
	QString UrlMessage = "mes=" + Message;
	QString UrlCharset = "charset=" + Charset;
	QString UrlFormat = "fmt=3";
	UrlString = UrlString + UrlLogin + '&' + UrlPassword + '&' + UrlPhone + '&' + UrlMessage + '&' + UrlCharset + '&' + UrlFormat;
	return UrlString;
}
//-----------------------------------------------------------------------------
