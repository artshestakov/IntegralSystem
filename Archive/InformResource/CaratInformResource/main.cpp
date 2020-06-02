#include "ISCoreInformResource.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreInformResource CoreInformResource(argc, argv);
	if (CoreInformResource.Invoke())
	{
		return CoreInformResource.exec();
	}
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
