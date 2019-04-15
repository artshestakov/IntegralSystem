#include "StdAfx.h"
#include "CGConfigurator.h"
#include "EXDefines.h"
#include "EXConstants.h"
#include "ISNamespace.h"
#include "ISLocalization.h"
#include "ISDebug.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "ISCommandLine.h"
#include "ISDatabase.h"
#include "ISLicense.h"
//-----------------------------------------------------------------------------
#include "CGConfiguratorCreate.h"
#include "CGConfiguratorUpdate.h"
#include "CGConfiguratorDelete.h"
#include "CGConfiguratorService.h"
#include "CGConfiguratorLicense.h"
#include "CGConfiguratorShow.h"
#include "CGConfiguratorFIAS.h"
//-----------------------------------------------------------------------------
void RegisterMetatype()
{
	qRegisterMetaType<CGConfiguratorCreate*>("CGConfiguratorCreate");
	qRegisterMetaType<CGConfiguratorUpdate*>("CGConfiguratorUpdate");
	qRegisterMetaType<CGConfiguratorDelete*>("CGConfiguratorDelete");
	qRegisterMetaType<CGConfiguratorService*>("CGConfiguratorService");
	qRegisterMetaType<CGConfiguratorLicense*>("CGConfiguratorLicense");
	qRegisterMetaType<CGConfiguratorShow*>("CGConfiguratorShow");
	qRegisterMetaType<CGConfiguratorFIAS*>("CGConfiguratorFIAS");
}
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	RegisterMetatype();
	CGConfigurator Configurator(argc, argv);
	ISNamespace::ConsoleArgumentType ArgumentType = Configurator.CheckArguments();
	if (ArgumentType == ISNamespace::CAT_Unknown)
	{
		ISDebug::ShowEmptyString();
		ISDebug::ShowString(LOCALIZATION("Configurator.InvalidArguments"));
		ISDebug::ShowString(LOCALIZATION("Configurator.InvalidArguments.Help"));
		ISCommandLine::Pause();
		return EXIT_CODE_ERROR;
	}

	if (!CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).length() || !CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).length())
	{
		ISDebug::ShowString(LOCALIZATION("Configurator.NotLoginOrPassword"));
		ISCommandLine::Pause();
		return EXIT_CODE_ERROR;
	}

    bool Connected = true;
    bool ExistDB = Configurator.CheckExistDatabase(Connected);
	if (ExistDB)
	{
        QString ErrorString;
        if (ISDatabase::GetInstance().ConnectToDefaultDB(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), ErrorString))
		{
			if (!ISLicense::GetInstance().Initialize())
			{
				ISDebug::ShowWarningString(ISLicense::GetInstance().GetErrorString());
			}
		}
		else
		{
			Connected = false;
		}
	}

    if (!Connected)
    {
        ISCommandLine::Pause();
        return EXIT_CODE_ERROR;
    }

	ISMetaData::GetInstanse().Initialize(ISLicense::GetInstance().GetName(), true, true);

	bool Executed = false;
	if (ArgumentType == ISNamespace::CAT_Interpreter)
	{
		ISDebug::ShowEmptyString();
		ISDebug::ShowString(LOCALIZATION("Configurator.Hello").arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE)).arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER)));
		ISDebug::ShowString(LOCALIZATION("Configurator.Hello.Help"));
		ISDebug::ShowString(LOCALIZATION("Configurator.Hello.Exit"));

		if (!ExistDB)
		{
			ISDebug::ShowEmptyString();
			ISDebug::ShowString(LOCALIZATION("Configurator.DatabaseNotExist").arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE)));
		}

		Configurator.InterpreterMode();
		return EXIT_CODE_NORMAL;
	}
	else if (ArgumentType == ISNamespace::CAT_OneArgument)
	{
		Executed = Configurator.Execute(Configurator.arguments().at(1).toLower());
	}
	else if (ArgumentType == ISNamespace::CAT_Standart)
	{
		Executed = Configurator.Execute(Configurator.arguments().at(1).toLower(), Configurator.arguments().at(2).toLower());
	}
	
	if (Executed)
	{
		return EXIT_CODE_NORMAL;
	}
	else
	{
		ISCommandLine::Pause();
		return EXIT_CODE_ERROR;
	}
}
//-----------------------------------------------------------------------------
