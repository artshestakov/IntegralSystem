#include "ISAsterisk.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISDebug.h"
#include "ISTcp.h"
//-----------------------------------------------------------------------------
ISAsterisk::ISAsterisk(QObject *parent)
	: QThread(parent),
	Port(0)
{

}
//-----------------------------------------------------------------------------
ISAsterisk::~ISAsterisk()
{

}
//-----------------------------------------------------------------------------
bool ISAsterisk::Start()
{
	Host = CONFIG_STRING(CONST_CONFIG_AMI_SERVER);
	if (Host.isEmpty())
	{
		ISLOGGER_E(__CLASS__, QString("Not specified config parameter: %1").arg(CONST_CONFIG_AMI_SERVER));
		return false;
	}

	Port = CONFIG_INT(CONST_CONFIG_AMI_PORT);
	if (Port < 1 || Port >= USHRT_MAX) //Если значение не входит в диапазон портов - использует порт по умолчанию
	{
		ISLOGGER_E(__CLASS__, QString("Invalid config value %1: %2.").arg(CONST_CONFIG_AMI_PORT).arg(Port));
		return false;
	}

	Login = CONFIG_STRING(CONST_CONFIG_AMI_LOGIN);
	if (Login.isEmpty())
	{
		ISLOGGER_E(__CLASS__, QString("Not specified config parameter: %1").arg(CONST_CONFIG_AMI_LOGIN));
		return false;
	}

	Password = CONFIG_STRING(CONST_CONFIG_AMI_PASSWORD);
	if (Password.isEmpty())
	{
		ISLOGGER_E(__CLASS__, QString("Not specified config parameter: %1").arg(CONST_CONFIG_AMI_PASSWORD));
		return false;
	}

	start();
	return true;
}
//-----------------------------------------------------------------------------
void ISAsterisk::run()
{
	TcpSocket = new QTcpSocket();
	connect(TcpSocket, &QTcpSocket::connected, this, &ISAsterisk::Connected, Qt::DirectConnection);
	connect(TcpSocket, &QTcpSocket::disconnected, this, &ISAsterisk::Disconnected, Qt::DirectConnection);
	connect(TcpSocket, static_cast<void(QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &ISAsterisk::Error, Qt::DirectConnection);
	connect(TcpSocket, &QTcpSocket::readyRead, this, &ISAsterisk::ReadyRead, Qt::DirectConnection);
	TcpSocket->connectToHost(Host, Port);
	exec();
}
//-----------------------------------------------------------------------------
void ISAsterisk::Connected()
{
	ISLOGGER_I(__CLASS__, "Connected");
	ActionLogin();
}
//-----------------------------------------------------------------------------
void ISAsterisk::Disconnected()
{

}
//-----------------------------------------------------------------------------
void ISAsterisk::Error(QAbstractSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
	ISLOGGER_E(__CLASS__, TcpSocket->errorString());
}
//-----------------------------------------------------------------------------
void ISAsterisk::ReadyRead()
{
	//Читаем данные из сокета
	Buffer.push_back(TcpSocket->readAll());

	int Pos = 0;
	while ((Pos = Buffer.indexOf(ASTERISK_AMI_SEPARATOR, 0)) != -1)
	{
		//Вытаскиваем пакет из буфера
		QString PackageString = Buffer.left(Pos);
		Buffer.remove(0, Pos + 4);

		//Обрабатываем пакет
		ISAmiPackage AMIPackage;
		QString PackageType, //Тип пакета
			PackageName; //Значение типа пакета
		QStringList PackageList = PackageString.split("\r\n");
		for (int i = 0, c = PackageList.size(); i < c; ++i)
		{
			QString Line = PackageList[i]; //Получаем текущую строку пакета
			if (!Line.contains(':')) //Если в строке нет разделителя - идём дальше
			{
				continue;
			}

			//Разделяем строку и добавляем её в вектор пакета
			QStringList LineList = Line.split(':');
			QString StringLeft = LineList.front(),
				StringRight = LineList.back();
			ISAlgorithm::RemoveBeginSymbolLoop(StringRight, SYMBOL_SPACE);
			AMIPackage.emplace_back(std::make_pair(StringLeft, StringRight));

			//Если сейчас первая строка - вытаскиваем тип пакета
			if (!i)
			{
				PackageType = StringLeft;
				PackageName = StringRight;
			}
		}

		if (PackageType == "Event")
		{
			if (PackageName == "SuccessfulAuth")
			{
				EventSuccessfulAuth(AMIPackage);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISAsterisk::SendAction(const QString &ActionType, const ISStringMap &StringMap)
{
	//Формируем сообщение
	QByteArray Content;
	Content += "Action: " + ActionType + "\r\n";
	for (const auto &MapItem : StringMap)
	{
		Content += MapItem.first + ": " + MapItem.second + "\r\n";
	}
	Content += "\r\n";

	//Отправляем его и ждём пока не отправится
	ISLOGGER_I(__CLASS__, "Sending action \"" + ActionType + "\"");
	TcpSocket->write(Content);
	ISTcp::WaitForBytesWritten(TcpSocket);
	ISLOGGER_I(__CLASS__, "Sended");
}
//-----------------------------------------------------------------------------
void ISAsterisk::ActionLogin()
{
	SendAction("Login", ISStringMap
	{
		{ "Username", Login },
		{ "Secret", Password }
	});
}
//-----------------------------------------------------------------------------
void ISAsterisk::EventSuccessfulAuth(const ISAmiPackage &AMIPackage)
{
	ISLOGGER_I(__CLASS__, "Successful auth");
}
//-----------------------------------------------------------------------------
