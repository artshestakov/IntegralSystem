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
	Dictionary.clear(); //������� �������
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
	if (Result) //���� ���������� ����������
	{
		QTranslator *Translator = new QTranslator(qApp);
		Result = Translator->load(FilePath);
		if (Result) //�������� ���������� ������ �������
		{
			Result = qApp->installTranslator(Translator);
			if (!Result) //�� ������� ���������� ����������
			{
				ErrorString = "not installing translator file: " + FilePath;
			}
		}
		else //�������� ���������� ���� ��������� � �������
		{
			ErrorString = "not load translator file: " + FilePath;
			delete Translator;
		}
	}
	else //���� ���������� �� ����������
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
	if (Result) //���� ���� ����������� ������� ������
	{
		Result = InitializeContent(File.readAll());
		File.close();
	}
	else //�� ������� ������� ���� �����������
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
	if (Result) //���� ���������� ����� �� ������
	{
		QDomElement DomElement = ISSystem::GetDomElement(Content);
		QDomNode NodeLocalization = DomElement.firstChild();

		QString LocalizationName = DomElement.attributes().namedItem("Name").nodeValue();
		Result = !LocalizationName.isEmpty();
		if (Result) //���� ��� ����������� �������
		{
			Result = !ISAlgorithm::VectorContains(LoadedFiles, LocalizationName);
			if (Result) //���� ���� ��� �� ���������������
			{
				while (!NodeLocalization.isNull() && Result)
				{
					if (!NodeLocalization.isComment())
					{
						//�������� ���� � �������� ��������
						QString LocalKey = NodeLocalization.attributes().namedItem("Name").nodeValue();
						QString Value = NodeLocalization.attributes().namedItem("Russian").nodeValue();

						Result = !LocalKey.isEmpty();
						if (Result) //���� ���� �������� �� ������
						{
							Result = !Value.isEmpty();
							if (Result) //�������� �������� �� ������ - ���������
							{
								Dictionary.emplace(LocalKey, Value);
							}
							else //�������� �������� ������
							{
								ErrorString = QString("localization value is empty. File: %1. Line: %2.").arg(LocalizationName).arg(NodeLocalization.lineNumber());
							}
						}
						else //���� �������� ������
						{
							ErrorString = QString("localization key is empty. File: %1. Line: %2.").arg(LocalizationName).arg(NodeLocalization.lineNumber());
						}
					}
					NodeLocalization = NodeLocalization.nextSibling();
				}
				LoadedFiles.emplace_back(LocalizationName);
			}
			else //���� ���� ��� ���������������
			{
				ErrorString = "already initialized.";
			}
		}
		else //��� ����������� �� �������
		{
			ErrorString = "empty localization name.";
		}
	}
	else //���������� ����� ������
	{
		ErrorString = "empty content.";
	}
	return Result;
}
//-----------------------------------------------------------------------------
