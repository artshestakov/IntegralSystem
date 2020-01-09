#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISFastAccessEntity.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISMenuFastAccess : public QMenu
{
	Q_OBJECT

signals:
	void CreateRecords();
	void ExternalTools();

public:
	ISMenuFastAccess(QWidget *parent = 0);
	virtual ~ISMenuFastAccess();

protected:
	void AboutToShow();
	void CreateRecord(QAction *ActionTriggered);
	void StartExternalTool(QAction *ActionTriggered);

private:
	QActionGroup *ActionGroupRecords;
	QActionGroup *ActionGroupTools;

	QAction *ActionCreateRecords;
	QAction *ActionExternalTools;
};
//-----------------------------------------------------------------------------
