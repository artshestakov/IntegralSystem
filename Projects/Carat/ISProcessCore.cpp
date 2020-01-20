#include "StdAfx.h"
#include "ISProcessCore.h"
//-----------------------------------------------------------------------------
ISProcessCore::ISProcessCore(const QString &name, const QString &local_name, const QString &core_file_name, QObject *parent)
	: QProcess(parent),
	Name(name),
	LocalName(local_name),
	IsRunning(false)
{
	setProgram(core_file_name);
	setReadChannel(ISProcessCore::StandardOutput);
	setProcessChannelMode(ISProcessCore::MergedChannels);
	connect(this, &ISProcessCore::stateChanged, this, &ISProcessCore::StateChanged);
}
//-----------------------------------------------------------------------------
ISProcessCore::~ISProcessCore()
{

}
//-----------------------------------------------------------------------------
QString ISProcessCore::GetName() const
{
	return Name;
}
//-----------------------------------------------------------------------------
QString ISProcessCore::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void ISProcessCore::SetRunning(bool running)
{
	IsRunning = running;
}
//-----------------------------------------------------------------------------
bool ISProcessCore::GetRunning() const
{
	return IsRunning;
}
//-----------------------------------------------------------------------------
void ISProcessCore::StateChanged(QProcess::ProcessState ProcessState)
{
	if (ProcessState == QProcess::NotRunning)
	{
		kill();
		emit Crash(Name);
	}
}
//-----------------------------------------------------------------------------
