#include "ISCoreAsteriskQueue.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreAsteriskQueue CoreAsteriskQueue(argc, argv);
	if (CoreAsteriskQueue.Invoke())
	{
		return CoreAsteriskQueue.exec();
	}
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
