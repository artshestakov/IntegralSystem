#include "ISDeviceComObject.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_DEVICE_USER = PREPARE_QUERY("SELECT cptp_name "
											  "FROM _deviceuser "
											  "LEFT JOIN _comporttype ON cptp_id = dvus_comport "
											  "WHERE dvus_id = :DeviceUserID");
//-----------------------------------------------------------------------------
ISDeviceComObject::ISDeviceComObject(int device_user_id, QObject *parent) : ISDeviceObjectBase(device_user_id, parent)
{
	SerialPort = new QSerialPort(this);
	connect(SerialPort, &QSerialPort::readyRead, this, &ISDeviceComObject::ReadyRead);
	connect(SerialPort, static_cast<void(QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &ISDeviceComObject::Error);
}
//-----------------------------------------------------------------------------
ISDeviceComObject::~ISDeviceComObject()
{
	
}
//-----------------------------------------------------------------------------
bool ISDeviceComObject::Initialize() const
{
	SerialPort->setPortName(GetPortName());
	return SerialPort->open(QIODevice::ReadOnly);
}
//-----------------------------------------------------------------------------
bool ISDeviceComObject::IsConnected() const
{
	for (QSerialPortInfo &SerialPortInfo : QSerialPortInfo::availablePorts())
	{
		if (SerialPortInfo.portName() == SerialPort->portName())
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
bool ISDeviceComObject::IsOpen() const
{
	return SerialPort->isOpen();
}
//-----------------------------------------------------------------------------
bool ISDeviceComObject::IsValid() const
{
	if (SerialPort->error() == QSerialPort::NoError)
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
QString ISDeviceComObject::GetErrorText() const
{
	return SerialPort->errorString();
}
//-----------------------------------------------------------------------------
void ISDeviceComObject::ReadyRead()
{
	BufferString.append(SerialPort->readAll());
	if (BufferString.contains("\r"))
	{
		BufferString = BufferString.replace("\r", QString());
		emit InputData(BufferString);
		BufferString.clear();
	}
}
//-----------------------------------------------------------------------------
void ISDeviceComObject::Error(QSerialPort::SerialPortError PortError)
{
	if (PortError == QSerialPort::ResourceError)
	{
		SerialPort->close();
	}
}
//-----------------------------------------------------------------------------
QString ISDeviceComObject::GetPortName() const
{
	QString PortName;

	ISQuery qSelect(QS_DEVICE_USER);
	qSelect.BindValue(":DeviceUserID", GetDeviceID());
	if (qSelect.ExecuteFirst())
	{
		PortName = qSelect.ReadColumn("cptp_name").toString();
	}

	return PortName;
}
//-----------------------------------------------------------------------------
