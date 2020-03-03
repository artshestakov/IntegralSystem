#include "ISCoreSMS.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreSMS CoreSMS(argc, argv);
	if (CoreSMS.Invoke())
	{
		return CoreSMS.exec();
	}
}
//-----------------------------------------------------------------------------
