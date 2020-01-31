#include "ISLocalization.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISCountingTime.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISLocalization::ISLocalization()
{
	QString TranslatorFileName = "qt_" + ISConfig::GetInstance().GetValueString(CONST_CONFIG_OTHER_LANGUAGE) + ".qm";
	QString TranslatorFilePath = APPLICATION_TRANSLATIONS_DIR + "/" + TranslatorFileName;

	if (QFile::exists(TranslatorFilePath))
	{
		QTranslator *Translator = new QTranslator(qApp);
		if (Translator->load(TranslatorFilePath))
		{
			if (qApp->installTranslator(Translator))
			{
				ISDebug::ShowInfoString(QString("Translator \"%1\" installing done").arg(TranslatorFileName));
			}
			else
			{
				ISDebug::ShowWarningString(QString("Translator \"%1\" not installing").arg(TranslatorFileName));
			}
		}
		else
		{
			ISDebug::ShowWarningString("Not load translator file " + TranslatorFilePath);
		}
	}
	else
	{
		ISDebug::ShowWarningString(QString("Not found translator file: %1").arg(TranslatorFilePath));
	}
}
//-----------------------------------------------------------------------------
ISLocalization::~ISLocalization()
{
	
}
//-----------------------------------------------------------------------------
ISLocalization& ISLocalization::GetInstance()
{
	static ISLocalization Localization;
	return Localization;
}
//-----------------------------------------------------------------------------
QString ISLocalization::GetString(const QString &ParameterName) const
{
	std::map<QString, QString>::const_iterator It = Dictionary.find(ParameterName);
	if (It == Dictionary.end())
	{
		return ParameterName;
	}
	return It->second;
}
//-----------------------------------------------------------------------------
void ISLocalization::LoadResourceFile(const QString &FileName)
{
	QFile File(":Localization/" + FileName + '.' + EXTENSION_LANG);
	IS_ASSERT(File.open(QIODevice::ReadOnly), QString("Not opened localization file \"%1\". Error: %2").arg(File.fileName()).arg(File.errorString()));
	InitializeContent(File.readAll());
	File.close();
}
//-----------------------------------------------------------------------------
void ISLocalization::InitializeContent(const QString &Content)
{
	if (Content.length())
	{
		ISCountingTime Time;
		int CountItems = 0;

		QDomElement DomElement = ISSystem::GetDomElement(Content);
		QDomNode NodeLocalization = DomElement.firstChild();

		QString LocalizationName = DomElement.attributes().namedItem("Name").nodeValue();
		IS_ASSERT(LocalizationName.length(), QString("Invalid name file localization. Name: %1").arg(LocalizationName));

		IS_ASSERT(!LoadedFiles.contains(LocalizationName), QString("Localization file \"%1\" already initialized.").arg(LocalizationName));

		while (!NodeLocalization.isNull())
		{
			QString LocalKey = NodeLocalization.attributes().namedItem("Name").nodeValue();
			QString Value = NodeLocalization.attributes().namedItem("Russian").nodeValue();

			if (LocalKey.length())
			{
				//IS_ASSERT(!Localization.contains(LocalKey), QString("Key \"%1\" already exist in localization map. File: %2. Line: %3").arg(LocalKey).arg(LocalizationName).arg(NodeLocalization.lineNumber()));
				//Localization.insert(LocalKey, Value);

				IS_ASSERT(Dictionary.find(LocalKey) == Dictionary.end(), QString("Key \"%1\" already exist in localization map. File: %2. Line: %3").arg(LocalKey).arg(LocalizationName).arg(NodeLocalization.lineNumber()))
				Dictionary.emplace(LocalKey, Value);

				CountItems++;
			}

			NodeLocalization = NodeLocalization.nextSibling();
		}

		int Msec = Time.GetElapsed();
		ISDebug::ShowInfoString(QString("Localization \"%1\" Initialized. msec: %2. Items: %3").arg(LocalizationName).arg(Msec).arg(CountItems));

		LoadedFiles.append(LocalizationName);
	}
}
//-----------------------------------------------------------------------------
