#include <QtCore/QCoreApplication>
#include <stdio.h>
#include "ASLogger.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication CoreApplication(argc, argv);
	if (!ASLogger::Instance().Initialize())
	{
		ASLogger::Instance().Shutdown();
	}
	else
	{
		printf("%s\n", ASLogger::Instance().GetErrorString().c_str());
	}
	return CoreApplication.exec();
}
//-----------------------------------------------------------------------------
