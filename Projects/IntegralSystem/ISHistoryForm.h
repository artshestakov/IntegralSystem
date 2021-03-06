#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISHistoryForm : public ISInterfaceForm
{
    Q_OBJECT

public:
    ISHistoryForm(QWidget *parent = 0);
    virtual ~ISHistoryForm();

protected:
    void AfterShowEvent() override;
    void EscapeClicked();

private:
    void Search(const QVariant &Value);
    void Open(QListWidgetItem *ListWidgetItem);

private:
    ISListWidget *ListWidget;
    QLabel *LabelRowCount;
};
//-----------------------------------------------------------------------------
