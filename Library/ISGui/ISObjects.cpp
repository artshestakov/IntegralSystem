#include "ISObjects.h"
#include "ISAssert.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#include "ISEmpty.h"
#include "ISOilSphere.h"
//-----------------------------------------------------------------------------
ISObjects::ISObjects()
	: ObjectInterface(nullptr)
{
	qRegisterMetaType<ISEmpty::Object*>("ISEmpty::Object");
	qRegisterMetaType<ISOilSphere::Object*>("ISOilSphere::Object");
}
//-----------------------------------------------------------------------------
ISObjects::~ISObjects()
{
	if (ObjectInterface)
	{
		delete ObjectInterface;
	}
}
//-----------------------------------------------------------------------------
ISObjects& ISObjects::Instance()
{
	static ISObjects Objects;
	return Objects;
}
//-----------------------------------------------------------------------------
void ISObjects::Initialize(const QString &ConfigurationName)
{
	ObjectInterface = ISAlgorithm::CreatePointer<ISObjectInterface *>("IS" + ConfigurationName + "::Object");
	ObjectInterface->SetConfigurationName(ConfigurationName);
	ObjectInterface->RegisterMetaTypes();
}
//-----------------------------------------------------------------------------
ISObjectInterface* ISObjects::GetInterface()
{
	return ObjectInterface;
}
//-----------------------------------------------------------------------------
