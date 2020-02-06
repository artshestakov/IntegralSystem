#include "ISCoreMedTech.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCoreMedTech CoreMedTech(argc, argv);
	if (CoreMedTech.Invoke())
	{
		return CoreMedTech.Exec();
	}
}
//-----------------------------------------------------------------------------
