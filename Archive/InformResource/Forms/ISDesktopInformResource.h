#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISCalendarWidget.h"
#include "ISListViewWidget.h"
#include "ISUserEdit.h"
#include "ISTextEdit.h"
//-----------------------------------------------------------------------------
class ISDesktopInformResource : public QWidget
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDesktopInformResource(QWidget *parent = 0);
	virtual ~ISDesktopInformResource();

protected:
	void showEvent(QShowEvent *e);

	void CreateLeftPanel();
	void CreateCentralPanel();
	void CreateRightPanel();

	void Reload();
	void UpdateDayNote();

	void DoubleClickedDemo(const QSqlRecord &SqlRecord);
	void DoubleClickedOganization(const QSqlRecord &SqlRecord);
	void DoubleClickedCallOrganization(const QSqlRecord &SqlRecord);

private:
	int CurrentUserID;

	QVBoxLayout *MainLayout;
	QHBoxLayout *LayoutCentral;
	
	ISListViewWidget *ListViewDemoAccesses;
	ISListViewWidget *ListViewOrganizations;
	ISUserEdit *UserEdit;
	ISCalendarWidget *CalendarWidget;
	ISTextEdit *TextEdit;
	ISListViewWidget *ListViewAllCalls;
	ISListViewWidget *ListViewOrganizationCalls;
};
//-----------------------------------------------------------------------------
