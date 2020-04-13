#include "ISLocalization.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISLocalization::ISLocalization()
{
	QString TranslatorFilePath = ISDefines::Core::PATH_TRANSLATIONS_DIR + '/' + "qt_ru.qm";
	if (QFile::exists(TranslatorFilePath))
	{
		QTranslator *Translator = new QTranslator(qApp);
		if (Translator->load(TranslatorFilePath))
		{
			if (!qApp->installTranslator(Translator))
			{
				ISLOGGER_WARNING(QString("Translator \"%1\" not installing").arg(TranslatorFilePath));
			}
		}
		else
		{
			ISLOGGER_WARNING("Not load translator file " + TranslatorFilePath);
		}
	}
	else
	{
		ISLOGGER_WARNING(QString("Not found translator file: %1").arg(TranslatorFilePath));
	}
}
//-----------------------------------------------------------------------------
ISLocalization::~ISLocalization()
{
	
}
//-----------------------------------------------------------------------------
ISLocalization& ISLocalization::Instance()
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
	QFile File(":Localization/" + FileName + SYMBOL_POINT + EXTENSION_LANG);
	if (File.open(QIODevice::ReadOnly))
	{
		InitializeContent(File.readAll());
		File.close();
	}
	else
	{
		ISLOGGER_WARNING(QString("Not opened localization file \"%1\". Error: %2").arg(File.fileName()).arg(File.errorString()));
	}
}
//-----------------------------------------------------------------------------
void ISLocalization::InitializeContent(const QString &Content)
{
	if (!Content.isEmpty())
	{
		QDomElement DomElement = ISSystem::GetDomElement(Content);
		QDomNode NodeLocalization = DomElement.firstChild();

		QString LocalizationName = DomElement.attributes().namedItem("Name").nodeValue();
		if (!LocalizationName.isEmpty())
		{
			if (!ISAlgorithm::VectorContains(LoadedFiles, LocalizationName))
			{
				while (!NodeLocalization.isNull())
				{
					if (!NodeLocalization.isComment())
					{
						QString LocalKey = NodeLocalization.attributes().namedItem("Name").nodeValue();
						QString Value = NodeLocalization.attributes().namedItem("Russian").nodeValue();
						if (!LocalKey.isEmpty())
						{
							IS_ASSERT(Dictionary.find(LocalKey) == Dictionary.end(), QString("Key \"%1\" already exist in localization map. File: %2. Line: %3").arg(LocalKey).arg(LocalizationName).arg(NodeLocalization.lineNumber()))
								Dictionary.emplace(LocalKey, Value);
						}
						else
						{
							ISLOGGER_WARNING(QString("Localization key is empty. File: %1. Line: %2.").arg(LocalizationName).arg(NodeLocalization.lineNumber()));
						}
					}
					NodeLocalization = NodeLocalization.nextSibling();
				}
				LoadedFiles.emplace_back(LocalizationName);
			}
			else
			{
				ISLOGGER_WARNING(QString("Localization file \"%1\" already initialized.").arg(LocalizationName));
			}
		}
		else
		{
			ISLOGGER_WARNING(QString("Invalid name file localization. Name: %1").arg(LocalizationName));
		}
	}
}
//-----------------------------------------------------------------------------
