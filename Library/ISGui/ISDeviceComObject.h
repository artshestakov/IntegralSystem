#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISDeviceObjectBase.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDeviceComObject : public ISDeviceObjectBase
{
	Q_OBJECT

public:
	ISDeviceComObject(int device_user_id, QObject *parent = 0);
	virtual ~ISDeviceComObject();

	bool Initialize() const override;
	bool IsConnected() const override;
	bool IsOpen() const override;
	bool IsValid() const override;
	QString GetErrorText() const override;

protected:
	void ReadyRead();
	void Error(QSerialPort::SerialPortError PortError);
	QString GetPortName() const;

private:
	QSerialPort *SerialPort;
	QString BufferString;
};
//-----------------------------------------------------------------------------
