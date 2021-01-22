#pragma once
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "PMetaClass.h"
#include "ISScrollArea.h"
#include "ISButtons.h"
#include "ISFieldEdits.h"
#include "ISWaitWidget.h"
//-----------------------------------------------------------------------------
class ISFullTextSearchSubSystem : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFullTextSearchSubSystem(QWidget *parent = 0);
	virtual ~ISFullTextSearchSubSystem();

	void LoadData() override;

private:
	void SetSearchInProgress(bool InProgress);
	void Search();
	void ClickedRecord();

private:
	QWidgetList WidgetList;

	ISSearchEdit *EditSearch;
	ISWaitWidget *WaitWidget;
	ISPushButton *ButtonSearch;
	QLabel *LabelResult;
	QFrame *Frame;
	ISScrollArea *ScrollArea;
};
//-----------------------------------------------------------------------------
