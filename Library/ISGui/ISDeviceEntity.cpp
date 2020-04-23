#include "ISDeviceEntity.h"
#include "ISAssert.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISCountingTime.h"
#include "ISSystem.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
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

			ISLOGGER_I(LANG("Device.Initialize.Process").arg(DeviceUserName));

			ISDeviceObjectBase *DeviceObjectBase = ISAlgorithm::CreatePointer<ISDeviceObjectBase *>(ClassName, Q_ARG(int, DeviceUserID), Q_ARG(QObject *, this));
			connect(DeviceObjectBase, &ISDeviceObjectBase::InputData, this, &ISDeviceEntity::InputData);
			Devices.insert(DeviceUserUID, DeviceObjectBase);
			DeviceObjectBase->Initialize() ?
				ISLOGGER_I(LANG("Device.Initialize.Done").arg(DeviceUserName)) :
				ISLOGGER_I(LANG("Device.Initialize.Error").arg(DeviceUserName) + ": " + DeviceObjectBase->GetErrorText());
		}
	}
}
//-----------------------------------------------------------------------------
ISDeviceObjectBase* ISDeviceEntity::GetDevice(const ISUuid &UID)
{
	return Devices.value(UID);
}
//-----------------------------------------------------------------------------
