#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISDeviceObjectBase.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDeviceEntity : public QObject
{
	Q_OBJECT

signals:
	void InputData(const QString &String);

public:
	ISDeviceEntity(const ISDeviceEntity &) = delete;
	ISDeviceEntity(ISDeviceEntity &&) = delete;
	ISDeviceEntity &operator=(const ISDeviceEntity &) = delete;
	ISDeviceEntity &operator=(ISDeviceEntity &&) = delete;
	~ISDeviceEntity();

	static ISDeviceEntity& GetInstance();

	void Initialize();
	ISDeviceObjectBase* GetDevice(const ISUuid &UID);

private:
	ISDeviceEntity();

	QMap<ISUuid, ISDeviceObjectBase*> Devices;
};
//-----------------------------------------------------------------------------
