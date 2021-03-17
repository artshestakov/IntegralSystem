#include "ISCaratApplication.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
	if (CaratApplication.Init())
	{
		return CaratApplication.Start();
	}
#ifdef DEBUG
	system("PAUSE");
#endif
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
