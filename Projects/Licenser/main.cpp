#include "StdAfx.h"
#include "EXDefines.h"
#include "EXLicense.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication CoreApplication(argc, argv);
	QStringList Arguments = CoreApplication.arguments();
	Arguments.removeAt(0);

	if (Arguments.count() == 3) //Если введены все аргументы
	{
		QString SerialUID = Arguments.at(0); //Уникальный идентификатор базы
		QString ConfUID = Arguments.at(1); //Уникальный идентификатор конфигурации
		QString FileName = Arguments.at(2); //Наименование файла лицензии

		QFile FileLicense(QString("%1/%2.%3").arg(APPLICATION_DIR_PATH).arg(FileName).arg(EXTENSION_LIC));
		if (FileLicense.exists())
		{
			FileLicense.remove();
		}

		if (FileLicense.open(QIODevice::WriteOnly))
		{
			FileLicense.write(EXLicense::GetKey(ConfUID, SerialUID));
			FileLicense.close();

			qDebug() << "File license write done:" << FileLicense.fileName();
		}
		else
		{
			qDebug() << "Error write license file:" << FileLicense.errorString();
		}
	}
	else //Аргументы введены неверно
	{
		qDebug() << "Invalid arguments!";
		qDebug() << "Usage: Licenser [SERIAL_UID] [CONFIGURATION_UID] [FILE_NAME]";
	}

	return CoreApplication.exec();
}
//-----------------------------------------------------------------------------
