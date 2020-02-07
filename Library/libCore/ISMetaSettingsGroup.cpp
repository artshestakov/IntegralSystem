#include "ISMetaSettingsGroup.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISMetaSettingsGroup::ISMetaSettingsGroup()
	: System(true)
{
	
}
//-----------------------------------------------------------------------------
ISMetaSettingsGroup::~ISMetaSettingsGroup()
{
	while (!Settings.isEmpty())
	{
		delete Settings.takeLast();
	}
}
//-----------------------------------------------------------------------------
