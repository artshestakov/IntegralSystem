#include "ISLicense.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "ISQuery.h"
#include "EXLicense.h"
#include "ISDebug.h"
#include "ISLocalization.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
static QString QS_TABLE = PREPARE_QUERY("SELECT COUNT(*) "
										"FROM pg_tables "
										"WHERE schemaname = current_schema() "
										"AND tablename = '_configurationlicense'");
//-----------------------------------------------------------------------------
static QString QS_LICENSE_EXIST = PREPARE_QUERY("SELECT COUNT(*) "
												"FROM _configurationlicense");
//-----------------------------------------------------------------------------
static QString QS_LICENSE = PREPARE_QUERY("SELECT clcn_uid, clcn_license "
										  "FROM _configurationlicense LIMIT 1");
//-----------------------------------------------------------------------------
static QString QD_LICENSE = PREPARE_QUERY("DELETE FROM _configurationlicense");
//-----------------------------------------------------------------------------
ISLicense::ISLicense()
{
	Initialized = false;

	QFile File(CONFIGURATION_SCHEME_PATH);
	IS_ASSERT(File.open(QIODevice::ReadOnly), File.errorString());
	QString Content = File.readAll();
	File.close();

	QDomElement DomElement = ISSystem::GetDomElement(Content);
	QDomNode DomNode = DomElement.firstChild();
	while (!DomNode.isNull()) //Обход конфигураций
	{
		ISLicenseItem *LicenseItem = new ISLicenseItem();
		LicenseItem->UID = DomNode.attributes().namedItem("UID").nodeValue();
		LicenseItem->Name = DomNode.attributes().namedItem("Name").nodeValue();
		LicenseItem->LocalName = DomNode.attributes().namedItem("LocalName").nodeValue();
		LicenseItem->ClassName = DomNode.attributes().namedItem("ClassName").nodeValue();
		LicenseItem->DesktopForm = DomNode.attributes().namedItem("DesktopForm").nodeValue();
		LicenseItem->IncomingCallForm = DomNode.attributes().namedItem("IncomingCallForm").nodeValue();
		VectorTemp.append(LicenseItem);
		
		DomNode = DomNode.nextSibling();
	}
}
//-----------------------------------------------------------------------------
ISLicense::~ISLicense()
{

}
//-----------------------------------------------------------------------------
ISLicense& ISLicense::GetInstance()
{
	static ISLicense License;
	return License;
}
//-----------------------------------------------------------------------------
QString ISLicense::GetName() const
{
	return ConfName;
}
//-----------------------------------------------------------------------------
QString ISLicense::GetLocalName() const
{
	return ConfLocalName;
}
//-----------------------------------------------------------------------------
QString ISLicense::GetClassName() const
{
	return ConfClassName;
}
//-----------------------------------------------------------------------------
QString ISLicense::GetDesktopForm() const
{
	return ConfDesktopForm;
}
//-----------------------------------------------------------------------------
QString ISLicense::GetIncomingCallForm() const
{
	return ConfIncomingCallForm;
}
//-----------------------------------------------------------------------------
QString ISLicense::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLicense::CheckExistLicenseTable() const
{
	bool Result = false;

	ISQuery qSelectTable(QS_TABLE);
	qSelectTable.SetShowLongQuery(false);
	if (qSelectTable.ExecuteFirst())
	{
		int Count = qSelectTable.ReadColumn("count").toInt();
		if (Count)
		{
			Result = true;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISLicense::CheckExistLicense() const
{
	bool Result = false;

	ISQuery qSelectCount(QS_LICENSE_EXIST);
	qSelectCount.SetShowLongQuery(false);
	if (qSelectCount.ExecuteFirst())
	{
		int Count = qSelectCount.ReadColumn("count").toInt();
		if (Count)
		{
			Result = true;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISLicense::Initialize()
{
	ISCountingTime CountingTime;

	if (!CheckExistLicenseTable())
	{
		ErrorString = LOCALIZATION("License.TableNotExist");
		return false;
	}

	if (!CheckExistLicense())
	{
		ErrorString = LOCALIZATION("License.NotExist");
		return false;
	}

	ISQuery qSelect(QS_LICENSE);
	qSelect.SetShowLongQuery(false);
	if (qSelect.ExecuteFirst())
	{
		ISUuid SerialUID = qSelect.ReadColumn("clcn_uid");
		QByteArray LicenseString = qSelect.ReadColumn("clcn_license").toByteArray();
		
		if (!LicenseString.length()) //Если лицензия не установлена
		{
			ErrorString = LOCALIZATION("License.Empty");
			return false;
		}

		if (!QRegExp(REG_EXP_LICENSE).exactMatch(LicenseString)) //Лицензия не корректная
		{
			Delete();
			ErrorString = LOCALIZATION("License.NotValid");
			return false;
		}

		for (ISLicenseItem *LicenseItem : VectorTemp) //Обход конфигураций
		{
			QByteArray ByteArray = EXLicense::GetKey(LicenseItem->UID, SerialUID);
			if (LicenseString == ByteArray) //Если лицензия активирована
			{
				Initialized = true;
				ConfName = LicenseItem->Name;
				ConfLocalName = LicenseItem->LocalName;
				ConfClassName = LicenseItem->ClassName;
				ConfDesktopForm = LicenseItem->DesktopForm;
				ConfIncomingCallForm = LicenseItem->IncomingCallForm;
				break;
			}
		}

		ClearItems();

		if (Initialized) //Лицензия найдена и инициализированна
		{
			ISDebug::ShowInfoString(LOCALIZATION("License.Done").arg(CountingTime.GetElapsed()).arg(ConfName));
		}
		else //Лицензия не соответствует ни одной из конфигураций
		{
			ErrorString = LOCALIZATION("License.NotFound");
			return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
void ISLicense::ClearItems()
{
	while (VectorTemp.count())
	{
		ISLicenseItem *LicenseItem = VectorTemp.takeFirst();
		delete LicenseItem;
	}
}
//-----------------------------------------------------------------------------
void ISLicense::Delete()
{
	ISQuery qDelete(QD_LICENSE);
	qDelete.SetShowLongQuery(false);
	qDelete.Execute();
}
//-----------------------------------------------------------------------------
