#include "ISObjects.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISMetaDataHelper.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#include "ISCenterSeven.h"
#include "ISDemo.h"
#include "ISHighway.h"
#include "ISInformResource.h"
#include "ISMedTech.h"
#include "ISOilSphere.h"
#include "ISPatriot.h"
#include "ISSirona.h"
//-----------------------------------------------------------------------------
ISObjects::ISObjects()
	: ErrorString(NO_ERROR_STRING),
	ObjectInterface(nullptr)
{
	qRegisterMetaType<ISCenterSeven*>("ISCenterSeven");
	qRegisterMetaType<ISDemo*>("ISDemo");
	qRegisterMetaType<ISHighway*>("ISHighway");
	qRegisterMetaType<ISInformResource*>("ISInformResource");
	qRegisterMetaType<ISMedTech*>("ISMedTech");
	qRegisterMetaType<ISOilSphere::Object*>("ISOilSphere::Object");
	qRegisterMetaType<ISPatriot*>("ISPatriot");
	qRegisterMetaType<ISSirona*>("ISSirona");
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
ISObjects& ISObjects::GetInstance()
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
bool ISObjects::IsInitialized() const
{
	return Info.IsValid;
}
//-----------------------------------------------------------------------------
bool ISObjects::Initialize()
{
	ConfigurationName = ISMetaDataHelper::GetConfigurationName(ErrorString);
	if (ConfigurationName.isEmpty())
	{
		return false;
	}

	QFile File(PATH_CONFIGURATIONS_SCHEME);
	bool Result = File.open(QIODevice::ReadOnly);
	if (Result)
	{
		QString Content = File.readAll();
		File.close();

		QDomElement DomElement = ISSystem::GetDomElement(Content);
		QDomNode DomNode = DomElement.firstChild();
		while (!DomNode.isNull())
		{
			QDomNamedNodeMap DomNamedNodeMap = DomNode.attributes();
			QString configuration_name = DomNamedNodeMap.namedItem("Name").nodeValue();
			if (configuration_name == ConfigurationName)
			{
				Info.IsValid = true;
				Info.UID = DomNamedNodeMap.namedItem("UID").nodeValue();
				Info.Name = configuration_name;
				Info.LocalName = DomNamedNodeMap.namedItem("LocalName").nodeValue();
				Info.DesktopForm = DomNamedNodeMap.namedItem("DesktopForm").nodeValue();
				Info.IncomingCallForm = DomNamedNodeMap.namedItem("IncomingCallForm").nodeValue();
				break;
			}
			DomNode = DomNode.nextSibling();
		}

		Result = Info.IsValid;
		if (!Result)
		{
			ErrorString = "Not found configuration";
		}
	}
	else
	{
		ErrorString = QString("Error read file \"%1\": %2").arg(File.fileName()).arg(File.errorString());
	}

	if (Result)
	{
		ObjectInterface = ISAlgorithm::CreatePointer<ISObjectInterface *>("IS" + Info.Name + "::Object");
		ObjectInterface->SetConfigurationName(ConfigurationName);
		ObjectInterface->RegisterMetaTypes();
	}
	return Result;
}
//-----------------------------------------------------------------------------
ISObjectInterface* ISObjects::GetInterface()
{
	return ObjectInterface;
}
//-----------------------------------------------------------------------------
ISConfigurationItem ISObjects::GetInfo()
{
	return Info;
}
//-----------------------------------------------------------------------------
