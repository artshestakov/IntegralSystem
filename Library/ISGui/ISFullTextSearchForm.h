#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "PMetaClass.h"
#include "ISScrollArea.h"
#include "ISButtons.h"
#include "ISFieldEdits.h"
#include "ISWaitWidget.h"
//-----------------------------------------------------------------------------
class ISFullTextSearchForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFullTextSearchForm(QWidget *parent = 0);
	virtual ~ISFullTextSearchForm();

	void LoadData() override;

private:
	void SetSearchInProgress(bool InProgress);
	void Search();
	void ClickedRecord();

private:
	QWidgetList WidgetList;

	ISLineEdit *LineEdit;
	ISWaitWidget *WaitWidget;
	ISPushButton *ButtonSearch;
	QLabel *LabelResult;
	QFrame *Frame;
	ISScrollArea *ScrollArea;
};
//-----------------------------------------------------------------------------
