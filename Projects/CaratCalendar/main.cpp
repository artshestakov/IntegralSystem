#include "ISCoreCalendar.h"
#include "ISLogger.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
void Test()
{
	//QCoreApplication::quit();
	QObject *o = nullptr;
	o->objectName();
}
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreCalendar CoreCalendar(argc, argv);
	if (CoreCalendar.Invoke())
	{
		QTimer::singleShot(3000, Test);
		return CoreCalendar.exec();
	}
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
