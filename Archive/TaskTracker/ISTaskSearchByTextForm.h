#pragma once
#ifndef _ISTASKSEARCHBYTEXTFORM_H_INCLUDED
#define _ISTASKSEARCHBYTEXTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceForm.h"
#include "ISButtons.h"
#include "ISListWidget.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISTaskSearchByTextForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISTaskSearchByTextForm(QWidget *parent = 0);
	virtual ~ISTaskSearchByTextForm();

protected:
	void EscapeClicked() override;
	void EnterClicked() override;

private:
	void EditValueChanged();
	void Search();
	void ItemDoubleClicked(QListWidgetItem *ListWidgetItem);

private:
	ISSearchEdit *EditSearch;
	ISPushButton *ButtonSearch;
	ISListWidget *ListWidget;
	QLabel *LabelResult;
};
//-----------------------------------------------------------------------------
#endif
