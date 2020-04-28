#include "ISCoreScheduler.h"
//-----------------------------------------------------------------------------
ISCoreScheduler::ISCoreScheduler(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreScheduler::~ISCoreScheduler()
{

}
//-----------------------------------------------------------------------------
bool ISCoreScheduler::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		Timer = new QTimer(this);
		Timer->setInterval(1000 * 60 * 60); //1 час
		connect(Timer, &QTimer::timeout, this, &ISCoreScheduler::Timeout);
		Timeout();
		Started();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreScheduler::Timeout()
{
	Timer->stop();
	//...
	Timer->start();
}
//-----------------------------------------------------------------------------
