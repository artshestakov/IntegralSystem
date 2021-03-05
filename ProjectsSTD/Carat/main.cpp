#include "ISCaratApplication.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
	if (CaratApplication.Init())
	{
		return CaratApplication.Start();
	}
	//logging error...
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
