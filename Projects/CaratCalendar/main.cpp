#include "ISCoreCalendar.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreCalendar CoreCalendar(argc, argv);
	if (CoreCalendar.Invoke())
	{
		return CoreCalendar.exec();
	}
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
