#include "StdAfx.h"
#include "ISCoreScheduler.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreScheduler CoreScheduler(argc, argv);
	if (CoreScheduler.Invoke())
	{
		return CoreScheduler.Exec();
	}
}
//-----------------------------------------------------------------------------
