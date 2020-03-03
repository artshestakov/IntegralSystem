#include "ISCoreNotification.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreNotification CoreNotification(argc, argv);
	if (CoreNotification.Invoke())
	{
		return CoreNotification.exec();
	}
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
