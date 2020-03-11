#include "ISObjects.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISMetaDataHelper.h"
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
	qRegisterMetaType<ISOilSphere*>("ISOilSphere");
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
	ConfigurationName = ISMetaDataHelper::GetConfigurationName();

	QFile File(PATH_CONFIGURATION_SCHEME);
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
				Info.ClassName = DomNamedNodeMap.namedItem("ClassName").nodeValue();
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

	if (Result)
	{
		int ObjectType = QMetaType::type((Info.ClassName + SYMBOL_STAR).toLocal8Bit().constData());
		if (ObjectType)
		{
			const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
			if (MetaObject)
			{
				ObjectInterface = dynamic_cast<ISObjectInterface*>(MetaObject->newInstance());
				if (ObjectInterface)
				{
					ObjectInterface->SetConfigurationName(ConfigurationName);
					ObjectInterface->RegisterMetaTypes();
				}
				else
				{
					ErrorString = QString("Error instance configuration. ClassName: %1.").arg(Info.ClassName);
				}
			}
			else
			{
				ErrorString = "Error opening subsystem widget.";
			}
		}
		else
		{
			ErrorString = QString("Class for configuration is null. ClassName: %1.").arg(Info.ClassName);
		}
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
