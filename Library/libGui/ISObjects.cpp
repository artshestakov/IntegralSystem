#include "ISObjects.h"
#include "ISLicense.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
#include "ISCenterSeven.h"
#include "ISDemo.h"
#include "ISFrame.h"
#include "ISHighway.h"
#include "ISInformResource.h"
#include "ISMedTech.h"
#include "ISPatriot.h"
#include "ISSirona.h"
//-----------------------------------------------------------------------------
ISObjects::ISObjects()
	: QObject(),
	ObjectInterface(nullptr)
{
	qRegisterMetaType<ISCenterSeven*>("ISCenterSeven");
	qRegisterMetaType<ISDemo*>("ISDemo");
	qRegisterMetaType<ISFrame*>("ISFrame");
	qRegisterMetaType<ISHighway*>("ISHighway");
	qRegisterMetaType<ISInformResource*>("ISInformResource");
	qRegisterMetaType<ISMedTech*>("ISMedTech");
	qRegisterMetaType<ISPatriot*>("ISPatriot");
	qRegisterMetaType<ISSirona*>("ISSirona");
}
//-----------------------------------------------------------------------------
ISObjects::~ISObjects()
{

}
//-----------------------------------------------------------------------------
ISObjects& ISObjects::GetInstance()
{
	static ISObjects Objects;
	return Objects;
}
//-----------------------------------------------------------------------------
void ISObjects::Initialize()
{
	QString ClassName = ISLicense::GetInstance().GetClassName();

	int ObjectType = QMetaType::type((ClassName + '*').toLocal8Bit().constData());
	IS_ASSERT(ObjectType, QString("Class for configuration is null. ClassName: %1.").arg(ClassName));

	const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
	IS_ASSERT(MetaObject, "Error opening subsystem widget.");

	ObjectInterface = dynamic_cast<ISObjectInterface*>(MetaObject->newInstance(Q_ARG(QObject *, this)));
	IS_ASSERT(ObjectInterface, QString("Error instance configuration. ClassName: %1.").arg(ClassName));
}
//-----------------------------------------------------------------------------
ISObjectInterface* ISObjects::GetInterface()
{
	return ObjectInterface;
}
//-----------------------------------------------------------------------------
