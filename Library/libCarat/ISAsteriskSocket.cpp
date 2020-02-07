#include "ISAsteriskSocket.h"
#include "ISSettingsDatabase.h"
#include "ISDebug.h"
#include "ISQuery.h"
#include "ISConstants.h"
#include "ISQueryText.h"
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

	ISDebug::ShowInfoString("Append filter for event: " + EventName);
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

		ISDebug::ShowString("Redirect to number \"" + Pattern + "\". Channel: " + Channel);
		write(QString("Action: Redirect\r\n").toUtf8().data());
		write(QString("Channel: %1\r\n").arg(Channel).toUtf8().data());
		write(QString("Exten: %1\r\n").arg(Pattern).toUtf8().data());
		write(QString("Context: %1\r\n").arg(Context).toUtf8().data());
		write(QString("Priority: 1\r\n").toUtf8().data());
		write("\r\n");
	}
	else
	{
		ISDebug::ShowString("Forwarding is not possible, outgoing context is not specified for number: " + Pattern);
	}
}
//-----------------------------------------------------------------------------
void ISAsteriskSocket::ConnectedHost()
{
	if (Timer->isActive())
	{
		Timer->stop();
	}

	ISDebug::ShowInfoString("Connecting to Asterisk Manager Interface...");
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
	case QAbstractSocket::UnconnectedState: ISDebug::ShowInfoString("Unconnected"); break;
	case QAbstractSocket::HostLookupState: ISDebug::ShowInfoString("Host lookup"); break;
	case QAbstractSocket::ConnectingState: ISDebug::ShowInfoString("Connecting"); break;
	case QAbstractSocket::ConnectedState: ISDebug::ShowInfoString("Connected"); break;
	case QAbstractSocket::BoundState: ISDebug::ShowInfoString("Bound"); break;
	case QAbstractSocket::ClosingState: ISDebug::ShowInfoString("Closing"); break;
	case QAbstractSocket::ListeningState: ISDebug::ShowInfoString("Listening"); break;
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
			ISDebug::ShowString("Connected to Asterisk Manager Interface");
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
	ISDebug::ShowDebugString("Reconnect to Asterisk Manager Interface...");
	Connect();
}
//-----------------------------------------------------------------------------
