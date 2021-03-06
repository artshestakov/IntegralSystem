#include "ISAsterisk.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISDebug.h"
#include "ISTcp.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISTrace.h"
//-----------------------------------------------------------------------------
static QString QI_ASTERISK_CALLS = PREPARE_QUERY("INSERT INTO _asteriskcalls("
	"ascl_source, "
	"ascl_destination, "
	"ascl_callerid, "
	"ascl_channel, "
	"ascl_destinationchannel, "
	"ascl_starttime, "
	"ascl_answertime, "
	"ascl_endtime, "
	"ascl_duration, "
	"ascl_billableseconds, "
	"ascl_disposition, "
	"ascl_uniqueid, "
	"ascl_userfield) "
	"VALUES("
	":Source, "
	":Destination, "
	":CallerID, "
	":Channel, "
	":DestinationChannel, "
	"to_timestamp(:StartTime, 'YYYY-MM-DD HH24:MI-SS'), "
	"to_timestamp(:AnswerTime, 'YYYY-MM-DD HH24:MI-SS'), "
	"to_timestamp(:EndTime, 'YYYY-MM-DD HH24:MI-SS'), "
	"COALESCE(:Duration, 0), "
	"COALESCE(:BillableSeconds, 0), "
	"(SELECT dstp_id FROM _dispositiontype WHERE dstp_name = :Disposition), "
	":UniqueID, "
	":UserField)");
//-----------------------------------------------------------------------------
ISAsterisk::ISAsterisk(QObject *parent)
	: QThread(parent),
	Pos(0),
	IsFirstPackage(true),
	TcpSocket(nullptr),
	Port(0),
	qInsert(nullptr)
{

}
//-----------------------------------------------------------------------------
ISAsterisk::~ISAsterisk()
{

}
//-----------------------------------------------------------------------------
bool ISAsterisk::Start()
{
	//�������� ����������� ���������
	Host = CONFIG_STRING(CONST_CONFIG_AMI_SERVER);
	Port = CONFIG_INT(CONST_CONFIG_AMI_PORT);
	Login = CONFIG_STRING(CONST_CONFIG_AMI_LOGIN);
	Password = CONFIG_STRING(CONST_CONFIG_AMI_PASSWORD);
	start();
	return true;
}
//-----------------------------------------------------------------------------
void ISAsterisk::run()
{
	//��� �������� ����������� ������ ��������� �� ������ �������, ����� ������������ ��� ������
	if (ISDatabase::Instance().Connect(CONNECTION_ASTERISK, ISDatabase::Instance().GetOption(CONNECTION_DEFAULT)))
	{
		qInsert = new ISQuery(ISDatabase::Instance().GetDB(CONNECTION_ASTERISK), QI_ASTERISK_CALLS);
	}
	else
	{
		ISLOGGER_E(__CLASS__, "Not connected to database: " + ISDatabase::Instance().GetErrorString());
		return;
	}

	TcpSocket = new QTcpSocket();
	connect(TcpSocket, &QTcpSocket::connected, this, &ISAsterisk::Connected, Qt::DirectConnection);
	connect(TcpSocket, &QTcpSocket::disconnected, this, &ISAsterisk::Connect, Qt::DirectConnection);
	connect(TcpSocket, static_cast<void(QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &ISAsterisk::Error, Qt::DirectConnection);
	connect(TcpSocket, &QTcpSocket::readyRead, this, &ISAsterisk::ReadyRead, Qt::DirectConnection);
	Connect();
	exec();

	//����������� �� AMI, ������� ��������� �� ����� � ������ �������
	disconnect(TcpSocket, &QTcpSocket::disconnected, this, &ISAsterisk::Connect); //����� ����� �� ����������� ���������������
	TcpSocket->disconnectFromHost();
	ISTcp::WaitForDisconnected(TcpSocket); //��� ����������
	delete TcpSocket;
	delete qInsert;
}
//-----------------------------------------------------------------------------
void ISAsterisk::Connect()
{
	ISLOGGER_I(__CLASS__, QString("Connecting to %1:%2").arg(Host).arg(Port));
	TcpSocket->connectToHost(Host, Port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
}
//-----------------------------------------------------------------------------
void ISAsterisk::Connected()
{
	ISLOGGER_I(__CLASS__, "Connected");
	ActionLogin();
}
//-----------------------------------------------------------------------------
void ISAsterisk::Error(QAbstractSocket::SocketError socket_error)
{
	ISLOGGER_E(__CLASS__, TcpSocket->errorString());
	if (socket_error == QAbstractSocket::ConnectionRefusedError || socket_error == QAbstractSocket::NetworkError) //���������� �� ������ ������������, ������� �������� ������������ ��� ���
	{
		Connect();
	}
	else if (socket_error == QAbstractSocket::RemoteHostClosedError) //����� ���������� �� �� ����� ���� - ��� ���� ������ (������ Connect ��� ��������� �� ������� disconnected)
	{
		ISSleep(5000);
	}
}
//-----------------------------------------------------------------------------
void ISAsterisk::ReadyRead()
{
	//������ ������ �� ������
	Buffer.push_back(TcpSocket->readAll());

	//���� ��� ������ ����� - ������� �������������� ��������� �� AMI
	if (IsFirstPackage)
	{
		Pos = Buffer.indexOf("\r\n");
		if (Pos != -1)
		{
			Buffer.remove(0, Pos + 2);
		}
		IsFirstPackage = false;
	}

	while ((Pos = Buffer.indexOf(ASTERISK_AMI_SEPARATOR, 0)) != -1)
	{
		//����������� ����� �� ������
		QString PackageString = Buffer.left(Pos);
		Buffer.remove(0, Pos + 4);

		//������������ �����
		ISStringMap AMIPackage;
		QString PackageType, //��� ������
			PackageName; //�������� ���� ������
		QStringList PackageList = PackageString.split("\r\n");
		for (int i = 0, c = PackageList.size(); i < c; ++i) //������� ��� ������ ������
		{
			QString Line = PackageList[i]; //�������� ������� ������ ������
			if (!Line.contains(':')) //���� � ������ ��� ����������� - ��� ������
			{
				continue;
			}

			//��������� ������ � ��������� � � ������ ������
			Pos = Line.indexOf(':', 0);
			QString StringLeft = Line.mid(0, Pos),
				StringRight = Line.mid(Pos + 1, Line.size() - Pos - 1);
			ISAlgorithm::RemoveBeginSymbolLoop(StringRight, SYMBOL_SPACE);
			AMIPackage.emplace(StringLeft, StringRight);
			if (!i) //���� ������ ������ ������ - ����������� ��� ������
			{
				PackageType = StringLeft;
				PackageName = StringRight;
			}
		}

		if (PackageType == "Event") //���� ����� �������� ��������
		{
			if (PackageName == "Cdr") //������� ����������
			{
				EventCDR(AMIPackage);
			}
		}
		else if (PackageType == "Response") //����� �������� ������� �� ������
		{
			QString ActionID = ExtractOfPackage(AMIPackage, "ActionID");
			if (ActionID == "Login") //����� �� �����������
			{
				ResponseLogin(AMIPackage);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISAsterisk::SendAction(const QString &ActionType, const ISStringMap &StringMap)
{
	//��������� ���������
	QString Content = "Action: " + ActionType + "\r\n";
	for (const auto &MapItem : StringMap)
	{
		Content += MapItem.first + ": " + MapItem.second + "\r\n";
	}
	Content += "\r\n";

	//���������� ��� � ��� ���� �� ����������
	ISLOGGER_I(__CLASS__, "Sending action \"" + ActionType + "\"");
	TcpSocket->write(Content.toUtf8());
	ISTcp::WaitForBytesWritten(TcpSocket);
}
//-----------------------------------------------------------------------------
const QString ISAsterisk::ExtractOfPackage(const ISStringMap &AMIPackage, const QString &Key) const
{
	auto It = AMIPackage.find(Key);
	if (It == AMIPackage.end())
	{
		return QString();
	}
	return It->second;
}
//-----------------------------------------------------------------------------
void ISAsterisk::ActionLogin()
{
	SendAction("Login", ISStringMap
	{
		{ "ActionID", "Login" },
		{ "Username", Login },
		{ "Secret", Password }
	});
}
//-----------------------------------------------------------------------------
void ISAsterisk::ActionFilter(const QString &EventName)
{
	SendAction("Filter", ISStringMap
	{
		{ "ActionID", "Filter" },
		{ "Operation", "Add" },
		{ "Filter", "Event: " + EventName }
	});
}
//-----------------------------------------------------------------------------
void ISAsterisk::ResponseLogin(const ISStringMap &AMIPackage)
{
	QString Message = "Response action \"Login\": " + ExtractOfPackage(AMIPackage, "Message");
	ExtractOfPackage(AMIPackage, "Response") == "Success" ? ISLOGGER_I(__CLASS__, Message) : ISLOGGER_E(__CLASS__, Message);
	ActionFilter("Cdr");
}
//-----------------------------------------------------------------------------
void ISAsterisk::EventCDR(const ISStringMap &AMIPackage)
{
    ISTRACE();
	QString Source = ExtractOfPackage(AMIPackage, "Source");
	QString Destination = ExtractOfPackage(AMIPackage, "Destination");
	QString CallerID = ExtractOfPackage(AMIPackage, "CallerID");
	QString Channel = ExtractOfPackage(AMIPackage, "Channel");
	QString DestinationChannel = ExtractOfPackage(AMIPackage, "DestinationChannel");
	QString StartTime = ExtractOfPackage(AMIPackage, "StartTime");
	QString AnswerTime = ExtractOfPackage(AMIPackage, "AnswerTime");
	QString EndTime = ExtractOfPackage(AMIPackage, "EndTime");
	QString Duration = ExtractOfPackage(AMIPackage, "Duration");
	QString BillableSeconds = ExtractOfPackage(AMIPackage, "BillableSeconds");
	QString Disposition = ExtractOfPackage(AMIPackage, "Disposition");
	QString UniqueID = ExtractOfPackage(AMIPackage, "UniqueID");
	QString UserField = ExtractOfPackage(AMIPackage, "UserField");

	qInsert->BindValue(":Source", Source.isEmpty() ? QVariant() : Source);
	qInsert->BindValue(":Destination", Destination);
	qInsert->BindValue(":CallerID", CallerID);
	qInsert->BindValue(":Channel", Channel);
	qInsert->BindValue(":DestinationChannel", DestinationChannel.isEmpty() ? QVariant() : DestinationChannel);
	qInsert->BindValue(":StartTime", StartTime);
	qInsert->BindValue(":AnswerTime", AnswerTime.isEmpty() ? QVariant() : AnswerTime);
	qInsert->BindValue(":EndTime", EndTime);
	qInsert->BindValue(":Duration", Duration);
	qInsert->BindValue(":BillableSeconds", BillableSeconds);
	qInsert->BindValue(":Disposition", Disposition);
	qInsert->BindValue(":UniqueID", UniqueID);
	qInsert->BindValue(":UserField", UserField.isEmpty() ? QVariant() : UserField);
	if (qInsert->Execute())
	{
		ISLOGGER_I(__CLASS__, "New call " + UniqueID);
	}
	else
	{
		ISLOGGER_E(__CLASS__, "Not inserted call: " + qInsert->GetErrorString());
	}
}
//-----------------------------------------------------------------------------
