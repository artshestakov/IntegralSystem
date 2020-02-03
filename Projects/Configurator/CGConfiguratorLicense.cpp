#include "CGConfiguratorLicense.h"
#include "ISQuery.h"
#include "ISDebug.h"
#include "ISCommandLine.h"
//-----------------------------------------------------------------------------
static QString QS_LICENSE_EXIST = PREPARE_QUERY("SELECT COUNT(*) FROM _configurationlicense");
//-----------------------------------------------------------------------------
static QString QI_LICENSE = PREPARE_QUERY("INSERT INTO _configurationlicense(clcn_license) VALUES(:License)");
//-----------------------------------------------------------------------------
static QString QS_LICENSE = PREPARE_QUERY("SELECT clcn_uid FROM _configurationlicense LIMIT 1");
//-----------------------------------------------------------------------------
static QString QU_LICENSE = PREPARE_QUERY("UPDATE _configurationlicense SET clcn_license = :License");
//-----------------------------------------------------------------------------
CGConfiguratorLicense::CGConfiguratorLicense() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorLicense::~CGConfiguratorLicense()
{

}
//-----------------------------------------------------------------------------
void CGConfiguratorLicense::create()
{
	ISQuery qSelect(QS_LICENSE_EXIST);
	qSelect.SetShowLongQuery(false);
	if (qSelect.ExecuteFirst())
	{
		int Count = qSelect.ReadColumn("count").toInt();
		if (Count)
		{
			ISDebug::ShowWarningString("License already exist");
		}
		else
		{
			ISQuery qInsert(QI_LICENSE);
			qInsert.SetShowLongQuery(false);
			qInsert.BindValue(":License", QVariant());
			if (qInsert.Execute())
			{
				ISDebug::ShowInfoString("License created");
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorLicense::show()
{
	ISQuery qSelect(QS_LICENSE_EXIST);
	qSelect.SetShowLongQuery(false);
	if (qSelect.ExecuteFirst())
	{
		int Count = qSelect.ReadColumn("count").toInt();
		if (Count)
		{
			ISQuery qSelectLicense(QS_LICENSE);
			qSelectLicense.SetShowLongQuery(false);
			if (qSelectLicense.ExecuteFirst())
			{
				ISUuid LicenseUID = qSelectLicense.ReadColumn("clcn_uid");
				ISDebug::ShowInfoString(QString("Serial UID: %1").arg(LicenseUID));
			}
		}
		else
		{
			ISDebug::ShowWarningString("License not exist");
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorLicense::update()
{
	ISQuery qSelect(QS_LICENSE_EXIST);
	qSelect.SetShowLongQuery(false);
	if (qSelect.ExecuteFirst())
	{
		int Count = qSelect.ReadColumn("count").toInt();
		if (Count)
		{
			ISDebug::ShowString("Enter the path to the license file:");
			QString FilePath = ISCommandLine::GetText(); //Путь к файлу лицензии

			if (FilePath.length()) //Путь к файлу введен
			{
				QFile FileLicense(FilePath);
				if (FileLicense.exists()) //Файл существует
				{
					if (FileLicense.open(QIODevice::ReadOnly)) //Файл открылся
					{
						QByteArray ByteArray = FileLicense.readAll();
						FileLicense.close();

						ISQuery qUpdateLicense(QU_LICENSE);
						qUpdateLicense.SetShowLongQuery(false);
						qUpdateLicense.BindValue(":License", ByteArray);
						if (qUpdateLicense.Execute())
						{
							ISDebug::ShowInfoString("License successfully updated");
						}
					}
					else //Файл не открылся
					{
						ISDebug::ShowWarningString(QString("License file %1 not open: %2").arg(FilePath).arg(FileLicense.errorString()));
					}
				}
				else //Файл не существует
				{
					ISDebug::ShowWarningString(QString("License file not exist: %1").arg(FilePath));
				}
			}
			else //Путь к файлу не введен
			{
				ISDebug::ShowWarningString("Empty file path!");
			}
		}
		else
		{
			ISDebug::ShowWarningString("License not exist");
		}
	}
}
//-----------------------------------------------------------------------------
