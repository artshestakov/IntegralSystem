#include "ISCoreHighway.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreHighway CaratCoreHighway(argc, argv);
	if (CaratCoreHighway.Invoke())
	{
		return CaratCoreHighway.exec();
	}
}
//-----------------------------------------------------------------------------
