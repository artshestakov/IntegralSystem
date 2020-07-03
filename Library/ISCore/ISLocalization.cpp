#include "ISLocalization.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISLocalization::ISLocalization()
	: ErrorString(NO_ERROR_STRING)
{
	QString TranslatorFilePath = ISDefines::Core::PATH_TRANSLATIONS_DIR + '/' + "qt_ru.qm";
	if (QFile::exists(TranslatorFilePath))
	{
		QTranslator *Translator = new QTranslator(qApp);
		if (Translator->load(TranslatorFilePath))
		{
			if (!qApp->installTranslator(Translator))
			{
				ISLOGGER_W(QString("Translator \"%1\" not installing").arg(TranslatorFilePath));
			}
		}
		else
		{
			ISLOGGER_W("Not load translator file " + TranslatorFilePath);
		}
	}
	else
	{
		ISLOGGER_W(QString("Not found translator file: %1").arg(TranslatorFilePath));
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
QString ISLocalization::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
QString ISLocalization::GetString(const QString &ParameterName) const
{
	std::map<QString, QString>::const_iterator It = Dictionary.find(ParameterName);
	return It == Dictionary.end() ? ParameterName : It->second;
}
//-----------------------------------------------------------------------------
bool ISLocalization::LoadResourceFile(const QString &FileName)
{
	QFile File(":Localization/" + FileName + SYMBOL_POINT + EXTENSION_LANG);
	bool Result = File.open(QIODevice::ReadOnly);
	if (Result) //Если файл локализации успешно открыт
	{
		Result = InitializeContent(File.readAll());
		File.close();
	}
	else //Не удалось открыть файл локализации
	{
		ErrorString = QString("not open localization file \"%1\". Error: %2.").arg(File.fileName()).arg(File.errorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISLocalization::InitializeContent(const QString &Content)
{
	bool Result = !Content.isEmpty();
	if (Result) //Если содержимое файла не пустое
	{
		QDomElement DomElement = ISSystem::GetDomElement(Content);
		QDomNode NodeLocalization = DomElement.firstChild();

		QString LocalizationName = DomElement.attributes().namedItem("Name").nodeValue();
		Result = !LocalizationName.isEmpty();
		if (Result) //Если имя локализации указано
		{
			Result = !ISAlgorithm::VectorContains(LoadedFiles, LocalizationName);
			if (Result) //Этот файл ещё не инициализирован
			{
				while (!NodeLocalization.isNull() && Result)
				{
					if (!NodeLocalization.isComment())
					{
						//Получаем ключ и значение перевода
						QString LocalKey = NodeLocalization.attributes().namedItem("Name").nodeValue();
						QString Value = NodeLocalization.attributes().namedItem("Russian").nodeValue();

						Result = !LocalKey.isEmpty();
						if (Result) //Если ключ перевода не пустой
						{
							Result = !Value.isEmpty();
							if (Result) //Значение перевода не пустое
							{
								Result = Dictionary.find(LocalKey) == Dictionary.end();
								if (Result) //Проверяем наличие такого ключа. Если его нет - добавляем в словарь, иначе - ошибка
								{
									Dictionary.emplace(LocalKey, Value);
								}
								else
								{
									ErrorString = QString("Key \"%1\" already exist in localization map. File: %2. Line: %3.").arg(LocalKey).arg(LocalizationName).arg(NodeLocalization.lineNumber());
								}									
							}
							else //Значение перевода пустое
							{
								ErrorString = QString("localization value is empty. File: %1. Line: %2.").arg(LocalizationName).arg(NodeLocalization.lineNumber());
							}
						}
						else //Ключ перевода пустой
						{
							ErrorString = QString("localization key is empty. File: %1. Line: %2.").arg(LocalizationName).arg(NodeLocalization.lineNumber());
						}
					}
					NodeLocalization = NodeLocalization.nextSibling();
				}
				LoadedFiles.emplace_back(LocalizationName);
			}
			else //Этот файл уже инициализирован
			{
				ErrorString = "already initialized.";
			}
		}
		else //Имя локализации не указано
		{
			ErrorString = "empty localization name.";
		}
	}
	else //Содержимое файла пустое
	{
		ErrorString = "empty content.";
	}
	return Result;
}
//-----------------------------------------------------------------------------
