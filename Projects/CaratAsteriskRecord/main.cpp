#include "StdAfx.h"
#include "ISCoreAsteriskRecord.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreAsteriskRecord CoreAsteriskRecord(argc, argv);
	if (CoreAsteriskRecord.Invoke())
	{
		return CoreAsteriskRecord.Exec();
	}
}
//-----------------------------------------------------------------------------
