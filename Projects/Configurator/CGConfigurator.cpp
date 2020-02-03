#include "StdAfx.h"
#include "CGConfigurator.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISDatabase.h"
#include "ISDebug.h"
#include "ISLocalization.h"
#include "ISConfig.h"
#include "CGConfiguratorBase.h"
#include "ISQueryException.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
CGConfigurator::CGConfigurator(int &argc, char **argv) : QCoreApplication(argc, argv)
{
	Q_INIT_RESOURCE(Resources);
	ISConfig::GetInstance().Initialize();
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CONFIGURATOR);
    ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_INTEGRAL_SYSTEM);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CORE);

	QFile File(CONFIGURATOR_SCHEME_PATH);
	IS_ASSERT(File.open(QIODevice::ReadOnly), File.errorString());
	QString Content = File.readAll();
	File.close();

	QDomElement DomElement = ISSystem::GetDomElement(Content);
	QDomNode NodeSections = DomElement.firstChild();

	while (!NodeSections.isNull()) //Обход разделов
	{
		QString SectionName = NodeSections.attributes().namedItem("Name").nodeValue();
		QString SectionClassName = NodeSections.attributes().namedItem("ClassName").nodeValue();

		CGSection *Section = new CGSection(this);
		Section->SetName(SectionName);
		Section->SetClassName(SectionClassName);
		Arguments.append(Section);

		QDomNode NodeFunctions = NodeSections.firstChild();
		while (!NodeFunctions.isNull()) //Обход подразделов
		{
			QString FunctionName = NodeFunctions.attributes().namedItem("FunctionName").nodeValue();
			QString FunctionLocalName = NodeFunctions.attributes().namedItem("FunctionLocalName").nodeValue();
			QString FunctionDescription = NodeFunctions.attributes().namedItem("Description").nodeValue();

			CGSectionItem *SectionItem = new CGSectionItem(Section);
			SectionItem->SetFunction(FunctionName);
			SectionItem->SetLocalName(FunctionLocalName);
			SectionItem->SetDescription(FunctionDescription);
			Section->AddItem(SectionItem);

			NodeFunctions = NodeFunctions.nextSibling();
		}

		NodeSections = NodeSections.nextSibling();
	}
}
//-----------------------------------------------------------------------------
CGConfigurator::~CGConfigurator()
{

}
//-----------------------------------------------------------------------------
void CGConfigurator::InterpreterMode()
{
	ISDebug::ShowEmptyString();
	ISDebug::ShowString(LANG("Configurator.InputCommand") + ':');

	std::string InputCommand;
	std::getline(std::cin, InputCommand);
	QString Command = QString::fromStdString(InputCommand);
	if (!Command.length())
	{
		Exit();
		return;
	}

	QStringList StringList = ParseInputCommand(Command);
	if (StringList.count() == 1)
	{
		if (StringList.at(0).toLower() == "exit" || StringList.at(0) == "quit") //Выход
		{
			Exit();
			return;
		}
		else if (StringList.at(0) == "restart") //Перезапуск
		{
			ISDebug::ShowString(LANG("Configurator.Restart") + "...");
			ISSystem::SleepMilliseconds(400);
			QProcess::startDetached(APPLICATION_FILE_PATH);
			return;
		}

		Execute(StringList.at(0));
	}
	else if (StringList.count() == 2)
	{
		Execute(StringList.at(0), StringList.at(1));
	}
	else
	{
		ISDebug::ShowString(LANG("Configurator.Command.NotFound"));
	}

	InterpreterMode();
}
//-----------------------------------------------------------------------------
bool CGConfigurator::CheckExistDatabase(bool &Connected)
{
	QString ErrorString;
	bool Exist = false;
	Connected = ISDatabase::GetInstance().ConnectToSystemDB(ErrorString);
	if (Connected)
	{
		Exist = ISDatabase::GetInstance().CheckExistDatabase(CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE));
		ISDatabase::GetInstance().DisconnectFromSystemDB();
	}
	else
	{
		ISDebug::ShowString(ErrorString);
	}
	return Exist;
}
//-----------------------------------------------------------------------------
bool CGConfigurator::Execute(const QString &Argument)
{
	bool Result = false;
	if (ISSystem::CheckExistSlot(this, Argument))
	{
		try
		{
			ISCountingTime CountingTime;
			Result = QMetaObject::invokeMethod(this, Argument.toUtf8().data());
			ISDebug::ShowString(LANG("Configurator.Command.Executed").arg(Argument).arg(ISSystem::MillisecondsToString(CountingTime.GetElapsed())));
		}
		catch (ISQueryException &QueryException) { }
	}
	else
	{
		ISDebug::ShowString(LANG("Configurator.Function.NotFound").arg(Argument));
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfigurator::Execute(const QString &Argument, const QString &SubArgument)
{
	QString ClassName = GetClassName(Argument);
	CGConfiguratorBase *CommandBase = nullptr;
	
	int ObjectType = QMetaType::type(ClassName.toLocal8Bit().constData());
	if (ObjectType)
	{
		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		if (MetaObject)
		{
			CommandBase = dynamic_cast<CGConfiguratorBase*>(MetaObject->newInstance());
			if (CommandBase)
			{
				if (ISSystem::CheckExistSlot(CommandBase, SubArgument))
				{
					connect(CommandBase, &CGConfiguratorBase::ProgressMessage, this, &CGConfigurator::ProgressMessage);
				}
				else
				{
					ISDebug::ShowString(LANG("Configurator.Function.NotFound").arg(SubArgument));
					return false;
				}
			}
			else
			{
				ISDebug::ShowString(LANG("Configurator.Class.NotFound").arg(Argument));
				return false;
			}
		}
		else
		{
			ISDebug::ShowString(LANG("Configurator.Class.NotFound").arg(Argument));
			return false;
		}
	}
	else
	{
		ISDebug::ShowString(LANG("Configurator.Class.NotFound").arg(Argument));
		return false;
	}
	
	bool Invoked = false;

	try
	{
		ISCountingTime CountingTime;
		Invoked = QMetaObject::invokeMethod(CommandBase, SubArgument.toLocal8Bit().constData());
		ISDebug::ShowString(LANG("Configurator.Commands.Executed").arg(Argument).arg(SubArgument).arg(ISSystem::MillisecondsToString(CountingTime.GetElapsed())));
	}
	catch (ISQueryException &e) { }

	if (CommandBase)
	{
		delete CommandBase;
		CommandBase = nullptr;
	}

	return Invoked;
}
//-----------------------------------------------------------------------------
ISNamespace::ConsoleArgumentType CGConfigurator::CheckArguments() const
{
	if (arguments().count() == 1) //Интерпретатор
	{
		return ISNamespace::CAT_Interpreter;
	}
	else if (arguments().count() == 2) //Один аргумент
	{
		return ISNamespace::CAT_OneArgument;
	}
	else if (arguments().count() == 3) //Два аргумента
	{
		return ISNamespace::CAT_Standart;
	}

	return ISNamespace::CAT_Unknown;
}
//-----------------------------------------------------------------------------
QString CGConfigurator::GetClassName(const QString &Argument)
{
	for (CGSection *Section : Arguments)
	{
		if (Section->GetName().toLower() == Argument)
		{
			return Section->GetClassName();
		}
	}

	return QString();
}
//-----------------------------------------------------------------------------
QStringList CGConfigurator::ParseInputCommand(const QString &Command)
{
	QStringList StringList;
	for (const QString &String : Command.split(SYMBOL_SPACE))
	{
		if (String.length())
		{
			StringList.append(String.toLower());
		}
	}

	return StringList;
}
//-----------------------------------------------------------------------------
void CGConfigurator::ProgressMessage(const QString &Message)
{
	ISDebug::ShowString(Message);
}
//-----------------------------------------------------------------------------
void CGConfigurator::Exit()
{
	ISDebug::ShowString(LANG("Configurator.CompletionWork") + "...");
	ISSystem::SleepMilliseconds(400);
}
//-----------------------------------------------------------------------------
void CGConfigurator::help()
{
	ISDebug::ShowString(LANG("Configurator.Help.Description"));
	ISDebug::ShowString(LANG("Configurator.Help.Using.Standart"));
	ISDebug::ShowString(LANG("Configurator.Help.Using.Personal"));
	ISDebug::ShowEmptyString();

	for (CGSection *Section : Arguments)
	{
		for (CGSectionItem *SectionItem : Section->GetItems())
		{
			ISDebug::ShowString(Section->GetName() + SYMBOL_SPACE + SectionItem->GetFunction() + " - " + SectionItem->GetDescription());
		}
	}
}
//-----------------------------------------------------------------------------
