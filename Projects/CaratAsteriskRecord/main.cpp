#include "ISCoreAsteriskRecord.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreAsteriskRecord CoreAsteriskRecord(argc, argv);
	if (CoreAsteriskRecord.Invoke())
	{
		return CoreAsteriskRecord.exec();
	}
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
