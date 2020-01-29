#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
ISCountingTime::ISCountingTime()
{
	Elapsed = 0;
	Time.start();
}
//-----------------------------------------------------------------------------
ISCountingTime::~ISCountingTime()
{

}
//-----------------------------------------------------------------------------
int ISCountingTime::GetElapsed()
{
	Elapsed = Time.elapsed();
	return Elapsed;
}
//-----------------------------------------------------------------------------
int ISCountingTime::GetElapsedRestart()
{
	Elapsed = Time.elapsed();
	Restart();
	return Elapsed;
}
//-----------------------------------------------------------------------------
void ISCountingTime::Restart()
{
	Time.restart();
}
//-----------------------------------------------------------------------------
