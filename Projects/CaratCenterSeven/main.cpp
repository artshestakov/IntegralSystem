#include "ISCoreCenterSeven.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreCenterSeven CoreCenterSeven(argc, argv);
	if (CoreCenterSeven.Invoke())
	{
		return CoreCenterSeven.exec();
	}
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
