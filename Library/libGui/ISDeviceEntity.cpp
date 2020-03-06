#include "ISDeviceEntity.h"
#include "ISAssert.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISCountingTime.h"
#include "ISSystem.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
static QString QS_DEVICE_USER = PREPARE_QUERY("SELECT dvus_id, dvus_uid, concat(dvtp_name, '/', dvce_name), dvce_class "
											  "FROM _deviceuser "
											  "LEFT JOIN _devicetype ON dvtp_id = dvus_type "
											  "LEFT JOIN _device ON dvce_id = dvus_device "
											  "WHERE NOT dvus_isdeleted "
											  "AND dvus_user = currentuserid()");
//-----------------------------------------------------------------------------
ISDeviceEntity::ISDeviceEntity() : QObject()
{
	
}
//-----------------------------------------------------------------------------
ISDeviceEntity::~ISDeviceEntity()
{

}
//-----------------------------------------------------------------------------
ISDeviceEntity& ISDeviceEntity::GetInstance()
{
	static ISDeviceEntity DeviceEntity;
	return DeviceEntity;
}
//-----------------------------------------------------------------------------
void ISDeviceEntity::Initialize()
{
	ISQuery qSelectDevice(QS_DEVICE_USER);
	if (qSelectDevice.Execute())
	{
		while (qSelectDevice.Next())
		{
			int DeviceUserID = qSelectDevice.ReadColumn("dvus_id").toInt();
			ISUuid DeviceUserUID = qSelectDevice.ReadColumn("dvus_uid");
			QString DeviceUserName = qSelectDevice.ReadColumn("concat").toString();
			QString ClassName = qSelectDevice.ReadColumn("dvce_class").toString();

			ISLOGGER_INFO(LANG("Device.Initialize.Process").arg(DeviceUserName));
			ISCountingTime Time;

			int ObjectType = QMetaType::type((ClassName + SYMBOL_STAR).toLocal8Bit().constData());
			IS_ASSERT(ObjectType, QString("Class for device is NULL. ClassName: %1").arg(ClassName));

			const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
			IS_ASSERT(MetaObject, "Invalid meta object for device class.");

			ISDeviceObjectBase *DeviceObjectBase = dynamic_cast<ISDeviceObjectBase*>(MetaObject->newInstance(Q_ARG(int, DeviceUserID), Q_ARG(QObject *, this)));
			IS_ASSERT(DeviceObjectBase, QString("Error instance device. ClassName: %1").arg(ClassName));

			connect(DeviceObjectBase, &ISDeviceObjectBase::InputData, this, &ISDeviceEntity::InputData);
			Devices.insert(DeviceUserUID, DeviceObjectBase);

			if (DeviceObjectBase->Initialize())
			{
				ISLOGGER_INFO(LANG("Device.Initialize.Done").arg(DeviceUserName).arg(ISSystem::MillisecondsToString(Time.Elapsed())));
			}
			else
			{
				ISLOGGER_INFO(LANG("Device.Initialize.Error").arg(DeviceUserName) + ": " + DeviceObjectBase->GetErrorText());
			}
		}
	}
}
//-----------------------------------------------------------------------------
ISDeviceObjectBase* ISDeviceEntity::GetDevice(const ISUuid &UID)
{
	return Devices.value(UID);
}
//-----------------------------------------------------------------------------
