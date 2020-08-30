#include "ISObjects.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISMetaDataHelper.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#include "ISEmpty.h"
#include "ISMedTech.h"
#include "ISOilSphere.h"
//-----------------------------------------------------------------------------
ISObjects::ISObjects()
	: ErrorString(NO_ERROR_STRING),
	ObjectInterface(nullptr),
	Valid(false)
{
	qRegisterMetaType<ISEmpty::Object*>("ISEmpty::Object");
	qRegisterMetaType<ISMedTech::Object*>("ISMedTech::Object");
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
bool ISObjects::IsInitialized() const
{
	return Valid;
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
				Info.UID = DomNamedNodeMap.namedItem("UID").nodeValue();
				Info.Name = configuration_name;
				Info.LocalName = DomNamedNodeMap.namedItem("LocalName").nodeValue();
				Info.DesktopForm = DomNamedNodeMap.namedItem("DesktopForm").nodeValue();
				Info.DateExpired = QDate::fromString(DomNamedNodeMap.namedItem("DateExpired").nodeValue(), FORMAT_DATE_V2);
				Info.LogoName = DomNamedNodeMap.namedItem("LogoName").nodeValue();
				Valid = true;
				break;
			}
			DomNode = DomNode.nextSibling();
		}
		Result = Valid;
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
