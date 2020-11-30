#include "ISObjects.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISMetaDataHelper.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#include "ISEmpty.h"
#include "ISOilSphere.h"
//-----------------------------------------------------------------------------
ISObjects::ISObjects()
	: ErrorString(NO_ERROR_STRING),
	ObjectInterface(nullptr)
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
QString ISObjects::GetErrorString() const
{
	return ErrorString;
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
