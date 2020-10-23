#include "ISCaratApplication.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
	if (!CaratApplication.Init())
	{
		return EXIT_FAILURE;
	}

	if (!CaratApplication.Run())
	{
		return EXIT_FAILURE;
	}
	
	return CaratApplication.exec();
}
//-----------------------------------------------------------------------------
