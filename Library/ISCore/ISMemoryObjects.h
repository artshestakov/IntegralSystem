#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMemoryObjects
{
public:
	ISMemoryObjects(const ISMemoryObjects &) = delete;
	ISMemoryObjects(ISMemoryObjects &&) = delete;
	ISMemoryObjects &operator=(const ISMemoryObjects &) = delete;
	ISMemoryObjects &operator=(ISMemoryObjects &&) = delete;
	~ISMemoryObjects();

	static ISMemoryObjects& GetInstance();

	void SetTabNavigation(QObject *tab_navigation);
	QObject* GetTabNavigation();

	void SetDesktop(QObject *desktop);
	QObject* GetDesktop();

	void SetWorkspaceForm(QObject *workspace_form);
	QObject* GetWorkspaceForm();
	
	void SetLogger(QObject *logger);
	QObject* GetLogger();

	void SetQueryThreader(QObject *query_threader);
	QObject* GetQueryThreader();

private:
	ISMemoryObjects();

	QObject *TabNavigation;
	QObject *Desktop;
	QObject *WorkspaceForm;
	QObject *Logger;
	QObject *QueryThreader;
};
//-----------------------------------------------------------------------------