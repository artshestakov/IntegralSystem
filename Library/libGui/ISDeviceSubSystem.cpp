#include "ISDeviceSubSystem.h"
//-----------------------------------------------------------------------------
ISDeviceSubSystem::ISDeviceSubSystem(QWidget *parent) : ISListBaseForm("Device", parent)
{
	GetQueryModel()->ClearClassFilter();
}
//-----------------------------------------------------------------------------
ISDeviceSubSystem::~ISDeviceSubSystem()
{

}
//-----------------------------------------------------------------------------
