#include "StdAfx.h"
#include "ISCoreMail.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreMail CoreMail(argc, argv);
	if (CoreMail.Invoke())
	{
		return CoreMail.Exec();
	}
}
//-----------------------------------------------------------------------------
