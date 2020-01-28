#include "ISAsteriskSocket.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISSettingsDatabase.h"
#include "ISLocalization.h"
#include "ISDebug.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_USER_CONTEXT = PREPARE_QUERY("SELECT aspt_context "
											   "FROM _asteriskpattern "
											   "WHERE aspt_pattern = :Pattern");
//-----------------------------------------------------------------------------
ISAsteriskSocket::ISAsteriskSocket(QObject *parent) : QTcpSocket(parent)
{
	Host = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_ASTERISK_SERVER);
	Port = SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_ASTERISK_PORT);
	Login = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_ASTERISK_LOGIN);
	Password = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_ASTERISK_PASSWORD);

	connect(this, &ISAsteriskSocket::connected, this, &ISAsteriskSocket::ConnectedHost);
	connect(this, &ISAsteriskSocket::disconnected, this, &ISAsteriskSocket::DisconnectedHost);
	connect(this, &ISAsteriskSocket::stateChanged, this, &ISAsteriskSocket::StateChangedHost);
	connect(this, static_cast<void(ISAsteriskSocket::*)(QAbstractSocket::SocketError)>(&ISAsteriskSocket::error), this, &ISAsteriskSocket::ErrorHost);
	connect(this, &ISAsteriskSocket::readyRead, this, &ISAsteriskSocket::ReadyRead);

	Timer = new QTimer(this);
	Timer->setInterval(ASTERISK_RECONNECT_TIMEOUT);
	connect(Timer, &QTimer::timeout, this, &ISAsteriskSocket::Timeout);
}
//-----------------------------------------------------------------------------
ISAsteriskSocket::~ISAsteriskSocket()
{

}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::Connect()
{
	connectToHost(Host, Port);
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::AddFilter(const QString &EventName)
{
	write(QString("Action: Filter\r\n").toUtf8().data());
	write(QString("Operation: Add\r\n").toUtf8().data());
	write(QString("Filter: Event: %1\r\n").arg(EventName).toUtf8().data());
	write("\r\n");

	ISDebug::ShowInfoString(LOCALIZATION("Telephony.AddFilter").arg(EventName));
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::Redirect(const QStringMap &StringMap, const QString &Pattern)
{
	ISQuery qSelectContext(QS_USER_CONTEXT);
	qSelectContext.SetShowLongQuery(false);
	qSelectContext.BindValue(":Pattern", Pattern);
	if (qSelectContext.ExecuteFirst())
	{
		QString Context = qSelectContext.ReadColumn("aspt_context").toString();
		QString Channel = StringMap.value("Channel");

		ISDebug::ShowString(LOCALIZATION("Telephony.Redirect").arg(Pattern).arg(Channel));
		write(QString("Action: Redirect\r\n").toUtf8().data());
		write(QString("Channel: %1\r\n").arg(Channel).toUtf8().data());
		write(QString("Exten: %1\r\n").arg(Pattern).toUtf8().data());
		write(QString("Context: %1\r\n").arg(Context).toUtf8().data());
		write(QString("Priority: 1\r\n").toUtf8().data());
		write("\r\n");
	}
	else
	{
		ISDebug::ShowString(LOCALIZATION("Telephony.RedirectFailed").arg(Pattern));
	}
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::ConnectedHost()
{
	if (Timer->isActive())
	{
		Timer->stop();
	}

	ISDebug::ShowInfoString(LOCALIZATION("Telephony.AuthToAMI.Process") + "...");
	write(QString("Action: Login\r\n").toUtf8().data());
	write(QString("Username: %1\r\n").arg(Login).toUtf8().data());
	write(QString("Secret: %1\r\n").arg(Password).toUtf8().data());
	write("\r\n");
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::DisconnectedHost()
{
	Timer->start();
	emit Disconnected();
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::StateChangedHost(QAbstractSocket::SocketState NewState)
{
	switch (NewState)
	{
	case QAbstractSocket::UnconnectedState: ISDebug::ShowInfoString(LOCALIZATION("QAbstractSocket.State.UnconnectedState")); break;
	case QAbstractSocket::HostLookupState: ISDebug::ShowInfoString(LOCALIZATION("QAbstractSocket.State.HostLookupState")); break;
	case QAbstractSocket::ConnectingState: ISDebug::ShowInfoString(LOCALIZATION("QAbstractSocket.State.ConnectingState")); break;
	case QAbstractSocket::ConnectedState: ISDebug::ShowInfoString(LOCALIZATION("QAbstractSocket.State.ConnectedState")); break;
	case QAbstractSocket::BoundState: ISDebug::ShowInfoString(LOCALIZATION("QAbstractSocket.State.BoundState")); break;
	case QAbstractSocket::ClosingState: ISDebug::ShowInfoString(LOCALIZATION("QAbstractSocket.State.ClosingState")); break;
	case QAbstractSocket::ListeningState: ISDebug::ShowInfoString(LOCALIZATION("QAbstractSocket.State.ListeningState")); break;
	}
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::ErrorHost(QAbstractSocket::SocketError ErrorHost)
{
    Q_UNUSED(ErrorHost);
	ISDebug::ShowDebugString(errorString());
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::ReadyRead()
{
	QVector<QStringMap> VectorEvent = ParseReadyRead(readAll());
	while (!VectorEvent.isEmpty()) //Обход всех событий происходящих в данный момент (если список событие не пустой)
	{
		QStringMap VariantMapEvent = VectorEvent.takeFirst(); //Забираем событие из списка событий
		QString EventName = VariantMapEvent.value("Event"); //Наименование события
		if (EventName == AMI_SUCCESSFUL_AUTH)
		{
			ISDebug::ShowString(LOCALIZATION("Telephony.AuthToAMI.Done"));
			emit SuccessfulAuth(VariantMapEvent);
		}
		else if (EventName == AMI_USER_EVENT)
		{
			emit UserEvent(VariantMapEvent);
		}
		else if (EventName == AMI_DIAL_BEGIN)
		{
			emit BeforeDialBegin(VariantMapEvent);
			emit DialBegin(VariantMapEvent);
			emit AfterDialBegin(VariantMapEvent);
		}
		else if (EventName == AMI_DIAL_END)
		{
			emit DialEnd(VariantMapEvent);
		}
		else if (EventName == AMI_BLIND_TRANSFER)
		{
			emit BlindTransfer(VariantMapEvent);
		}
		else if (EventName == AMI_HANGUP)
		{
			emit Hangup(VariantMapEvent);
		}
		else if (EventName == AMI_EXTENSION_STATUS)
		{
			emit ExtensionStatus(VariantMapEvent);
		}
	}
}
//-----------------------------------------------------------------------------
QVector<QStringMap> ISAsteriskSocket::ParseReadyRead(const QString &String)
{
	QVector<QStringMap> VectorEvent;

	QStringList MessageList = String.split("\r\n\r\n");
	for (int i = 0; i < MessageList.count(); ++i) //Обход всех сообщений
	{
		QString Message = MessageList.at(i);
		if (!Message.length())
		{
			continue;
		}

		QStringMap StringMap;
		QStringList StringList = Message.split("\n"); //Список строк сообщения

		while (StringList.count()) //Обход сообщения
		{
			QString LineString = StringList.takeFirst().simplified();
			QStringList LineList = LineString.split(": ");
			if (LineList.count() == 2)
			{
				StringMap.insert(LineList.at(0), LineList.at(1));
			}
		}

		if (StringMap.contains("Event")) //Если сообщение является событием
		{
			VectorEvent.append(StringMap);
		}
	}

	return VectorEvent;
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::Timeout()
{
	ISDebug::ShowDebugString(LOCALIZATION("Telephony.Reconnect"));
	Connect();
}
//-----------------------------------------------------------------------------
