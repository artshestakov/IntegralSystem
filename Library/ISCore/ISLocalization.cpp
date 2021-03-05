#include "ISLocalization.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
#include "tinyxml2.h"
//-----------------------------------------------------------------------------
ISLocalization::ISLocalization()
	: ErrorString(NO_ERROR_STRING)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISLocalization::~ISLocalization()
{
	Dictionary.clear(); //Очищаем словарь
	CRITICAL_SECTION_DESTROY(&CriticalSection);
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
QString ISLocalization::GetString(const QString &ParameterName)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	auto It = Dictionary.find(ParameterName);
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	if (It == Dictionary.end())
	{
		ISLOGGER_W(__CLASS__, "Not found key \"" + ParameterName + "\" in localization");
		return ParameterName;
	}
	return It->second;
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
bool ISLocalization::LoadFile(const std::string &FilePath)
{
	tinyxml2::XMLDocument Document;
	tinyxml2::XMLError Error = Document.LoadFile(FilePath.c_str());
	bool Result = Error == tinyxml2::XML_SUCCESS;
	if (Result) //Файл загрузился успешно - получаем главный элемент
	{
		tinyxml2::XMLElement *XmlElement = Document.FirstChildElement();
		Result = XmlElement ? true : false;
		if (Result) //Главные элемент есть - переходим к элементам локализации
		{
			QString LocalizationName = XmlElement->Attribute("Name"); //Получаем имя файла локализации
			XmlElement = XmlElement->FirstChildElement();
			Result = XmlElement ? true : false;
			if (Result) //Как минимум один элемент есть - обходим элементы локализации
			{
				do
				{
					//Получаем ключ и значение на русском языке
					std::string Key = XmlElement->Attribute("Name"),
						Value = ISAlgorithm::FromLocal8Bit(XmlElement->Attribute("Russian"));

					Result = !Key.empty();
					if (!Result) //Если ключ пустой - ошибка
					{
						ErrorString = QString("localization key is empty. File: %1. Line: %2.").arg(LocalizationName).arg(XmlElement->GetLineNum());
						break;
					}

					Result = !Value.empty();
					if (!Result) //Если значение пустое - ошибка
					{
						ErrorString = QString("localization value is empty. File: %1. Line: %2.").arg(LocalizationName).arg(XmlElement->GetLineNum());
						break;
					}

					Result = DictionarySTD.find(Key) == DictionarySTD.end();
					if (!Result)
					{
						ErrorString = QString("duplicate key \"%1\"").arg(QString::fromStdString(Key));
						break;
					}
					DictionarySTD.emplace(Key, Value);
				} while ((XmlElement = XmlElement->NextSiblingElement()) != nullptr);
			}
			else //Элементы локализации отсутствуют
			{
				ErrorString = "Not found local";
			}
		}
		else //Главного элемента нет
		{
			ErrorString = "Not exist main element LANG";
		}
	}
	else //Ошибка загрузки файла
	{
		ErrorString = Document.ErrorStr();
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
