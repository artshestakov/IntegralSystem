#include "ISCoreTelephony.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreTelephony CoreTelephony(argc, argv);
	if (CoreTelephony.Invoke())
	{
		return CoreTelephony.Exec();
	}
}
//-----------------------------------------------------------------------------
