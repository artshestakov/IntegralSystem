#include "ISLocalization.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISLocalization::ISLocalization()
	: ErrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
ISLocalization::~ISLocalization()
{
	Dictionary.clear(); //Очищаем словарь
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
	ISStringMap::const_iterator It = Dictionary.find(ParameterName);
	if (It == Dictionary.end())
	{
		ISLOGGER_W("Not found key \"" + ParameterName + "\" in localization");
		return ParameterName;
	}
	return It->second;
}
//-----------------------------------------------------------------------------
bool ISLocalization::LoadTraslatorQT()
{
	QString FilePath = ISDefines::Core::PATH_TRANSLATIONS_DIR + '/' + "qt_ru.qm";
	bool Result = QFile::exists(FilePath);
	if (Result) //Файл трансляций существует
	{
		QTranslator *Translator = new QTranslator(qApp);
		Result = Translator->load(FilePath);
		if (Result) //Загрузка трансляций прошла успешно
		{
			Result = qApp->installTranslator(Translator);
			if (!Result) //Не удалось установить трансляции
			{
				ErrorString = "not installing translator file: " + FilePath;
			}
		}
		else //Загрузка трансляций была завершена с ошибкой
		{
			ErrorString = "not load translator file: " + FilePath;
			delete Translator;
		}
	}
	else //Файл трансляций не существует
	{
		ErrorString = "Not found translator file: " + FilePath;
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISLocalization::LoadResourceFile(const QString &FileName)
{
	ISLOGGER_I("Initialize file: " + FileName);
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
		ISLOGGER_E(ErrorString);
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
							if (Result) //Значение перевода не пустое - добавляем
							{
								Dictionary.emplace(LocalKey, Value);
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
