#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDeviceObjectBase : public QObject
{
	Q_OBJECT

signals:
	void InputData(const QString &String);

public:
	ISDeviceObjectBase(int device_user_id, QObject *parent = 0);
	virtual ~ISDeviceObjectBase();

	virtual bool Initialize() const = 0;
	virtual bool IsConnected() const = 0;
	virtual bool IsOpen() const = 0;
	virtual bool IsValid() const = 0;
	virtual QString GetErrorText() const = 0;

protected:
	int GetDeviceID() const;

private:
	int DeviceUserID;
};
//-----------------------------------------------------------------------------
