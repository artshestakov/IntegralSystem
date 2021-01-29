#include "ISCaratMonitor.h"
//-----------------------------------------------------------------------------
ISCaratMonitor::ISCaratMonitor()
{

}
//-----------------------------------------------------------------------------
ISCaratMonitor::~ISCaratMonitor()
{

}
//-----------------------------------------------------------------------------
ISCaratMonitor& ISCaratMonitor::Instance()
{
	static ISCaratMonitor CaratMonitor;
	return CaratMonitor;
}
//-----------------------------------------------------------------------------
