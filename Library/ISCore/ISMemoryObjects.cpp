#include "ISMemoryObjects.h"
//-----------------------------------------------------------------------------
ISMemoryObjects::ISMemoryObjects()
	: TabNavigation(nullptr),
	Desktop(nullptr),
	WorkspaceForm(nullptr),
	Logger(nullptr),
	QueryThreader(nullptr)
{
	
}
//-----------------------------------------------------------------------------
ISMemoryObjects::~ISMemoryObjects()
{

}
//-----------------------------------------------------------------------------
ISMemoryObjects& ISMemoryObjects::GetInstance()
{
	static ISMemoryObjects MemoryObjects;
	return MemoryObjects;
}
//-----------------------------------------------------------------------------
void ISMemoryObjects::SetTabNavigation(QObject *tab_navigation)
{
	TabNavigation = tab_navigation;
}
//-----------------------------------------------------------------------------
QObject* ISMemoryObjects::GetTabNavigation()
{
	return TabNavigation;
}
//-----------------------------------------------------------------------------
void ISMemoryObjects::SetDesktop(QObject *desktop)
{
	Desktop = desktop;
}
//-----------------------------------------------------------------------------
QObject* ISMemoryObjects::GetDesktop()
{
	return Desktop;
}
//-----------------------------------------------------------------------------
void ISMemoryObjects::SetWorkspaceForm(QObject *workspace_form)
{
	WorkspaceForm = workspace_form;
}
//-----------------------------------------------------------------------------
QObject* ISMemoryObjects::GetWorkspaceForm()
{
	return WorkspaceForm;
}
//-----------------------------------------------------------------------------
void ISMemoryObjects::SetLogger(QObject *logger)
{
	Logger = logger;
}
//-----------------------------------------------------------------------------
QObject* ISMemoryObjects::GetLogger()
{
	return Logger;
}
//-----------------------------------------------------------------------------
void ISMemoryObjects::SetQueryThreader(QObject *query_threader)
{
	QueryThreader = query_threader;
}
//-----------------------------------------------------------------------------
QObject* ISMemoryObjects::GetQueryThreader()
{
	return QueryThreader;
}
//-----------------------------------------------------------------------------
