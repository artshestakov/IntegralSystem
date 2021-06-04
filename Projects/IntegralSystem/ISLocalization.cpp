#include "ISLocalization.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISAlgorithmOld.h"
#include "ISLogger.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISLocalization::ISLocalization()
	: ErrorString(NO_ERROR_STRING)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISLocalization::~ISLocalization()
{
	Dictionary.clear(); //������� �������
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
	if (Result) //���� ���� ����������� ������� ������
	{
		Result = InitializeContent(File.readAll());
		File.close();
	}
	else //�� ������� ������� ���� �����������
	{
		ErrorString = QString("not open localization file \"%1\". Error: %2.").arg(File.fileName()).arg(File.errorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISLocalization::InitializeContent(const QString &Content)
{
	bool Result = !Content.isEmpty();
	if (Result) //���� ���������� ����� �� ������
	{
		QDomElement DomElement = ISGui::GetDomElement(Content);
		QDomNode NodeLocalization = DomElement.firstChild();

		QString LocalizationName = DomElement.attributes().namedItem("Name").nodeValue();
		Result = !LocalizationName.isEmpty();
		if (Result) //���� ��� ����������� �������
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
