#include "ISCaratMonitor.h"
//-----------------------------------------------------------------------------
ISCaratMonitor::ISCaratMonitor()
	: _IncomingQuerySize(0),
	_AnswerQuerySize(0)
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
void ISCaratMonitor::IncomingQuerySize(int QuerySize)
{
	_IncomingQuerySize += QuerySize;
}
//-----------------------------------------------------------------------------
void ISCaratMonitor::AnswerQuerySize(int AnswerSize)
{
	_AnswerQuerySize += AnswerSize;
}
//-----------------------------------------------------------------------------
