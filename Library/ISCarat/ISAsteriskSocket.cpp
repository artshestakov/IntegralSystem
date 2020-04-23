#include "ISAsteriskSocket.h"
#include "ISSettingsDatabase.h"
#include "ISLogger.h"
#include "ISQuery.h"
#include "ISConstants.h"
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

	ISLOGGER_I("Append filter for event: " + EventName);
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::Redirect(const ISStringMap &StringMap, const QString &Pattern)
{
	ISQuery qSelectContext(QS_USER_CONTEXT);
	qSelectContext.SetShowLongQuery(false);
	qSelectContext.BindValue(":Pattern", Pattern);
	if (qSelectContext.ExecuteFirst())
	{
		QString Context = qSelectContext.ReadColumn("aspt_context").toString();
		QString Channel = StringMap.at("Channel");

		ISLOGGER_L("Redirect to number \"" + Pattern + "\". Channel: " + Channel);
		write(QString("Action: Redirect\r\n").toUtf8().data());
		write(QString("Channel: %1\r\n").arg(Channel).toUtf8().data());
		write(QString("Exten: %1\r\n").arg(Pattern).toUtf8().data());
		write(QString("Context: %1\r\n").arg(Context).toUtf8().data());
		write(QString("Priority: 1\r\n").toUtf8().data());
		write("\r\n");
	}
	else
	{
		ISLOGGER_L("Forwarding is not possible, outgoing context is not specified for number: " + Pattern);
	}
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::ConnectedHost()
{
	if (Timer->isActive())
	{
		Timer->stop();
	}

	ISLOGGER_I("Connecting to Asterisk Manager Interface...");
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
	case QAbstractSocket::UnconnectedState: ISLOGGER_I("Unconnected"); break;
	case QAbstractSocket::HostLookupState: ISLOGGER_I("Host lookup"); break;
	case QAbstractSocket::ConnectingState: ISLOGGER_I("Connecting"); break;
	case QAbstractSocket::ConnectedState: ISLOGGER_I("Connected"); break;
	case QAbstractSocket::BoundState: ISLOGGER_I("Bound"); break;
	case QAbstractSocket::ClosingState: ISLOGGER_I("Closing"); break;
	case QAbstractSocket::ListeningState: ISLOGGER_I("Listening"); break;
	}
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::ErrorHost(QAbstractSocket::SocketError ErrorHost)
{
    Q_UNUSED(ErrorHost);
	ISLOGGER_D(errorString());
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::ReadyRead()
{
	QVector<ISStringMap> VectorEvent = ParseReadyRead(readAll());
	while (!VectorEvent.isEmpty()) //Обход всех событий происходящих в данный момент (если список событие не пустой)
	{
		ISStringMap VariantMapEvent = VectorEvent.takeFirst(); //Забираем событие из списка событий
		QString EventName = VariantMapEvent["Event"]; //Наименование события
		if (EventName == AMI_SUCCESSFUL_AUTH)
		{
			ISLOGGER_L("Connected to Asterisk Manager Interface");
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
QVector<ISStringMap> ISAsteriskSocket::ParseReadyRead(const QString &String)
{
	QVector<ISStringMap> VectorEvent;

	QStringList MessageList = String.split("\r\n\r\n");
	for (const QString &Message : MessageList) //Обход всех сообщений
	{
		if (Message.isEmpty())
		{
			continue;
		}

		ISStringMap StringMap;
		QStringList StringList = Message.split("\n"); //Список строк сообщения

		while (StringList.count()) //Обход сообщения
		{
			QString LineString = StringList.takeFirst().simplified();
			QStringList LineList = LineString.split(": ");
			if (LineList.count() == 2)
			{
				StringMap.emplace(LineList.at(0), LineList.at(1));
			}
		}

		if (StringMap.count("Event")) //Если сообщение является событием
		{
			VectorEvent.append(StringMap);
		}
	}

	return VectorEvent;
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::Timeout()
{
	ISLOGGER_D("Reconnect to Asterisk Manager Interface...");
	Connect();
}
//-----------------------------------------------------------------------------
