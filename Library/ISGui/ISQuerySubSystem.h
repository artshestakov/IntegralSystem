#pragma once
#ifndef _ISQUERYSUBSYSTEM_H_INCLUDED
#define _ISQUERYSUBSYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "ISListWidget.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISQuerySubSystem : public ISInterfaceMetaForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISQuerySubSystem(QWidget *parent);
    virtual ~ISQuerySubSystem();

    void LoadData() override;

private:
    void SearchEvent(const QVariant &Value);
    void ItemClicked(QListWidgetItem *ListWidgetItem);
    void ClearHistory();

private:
    QGroupBox *GroupBoxQuery;
    ISSearchEdit *EditSearch;
    ISListWidget *ListWidgetQuery;
    ISTextEdit *TextEdit;
    ISListWidget *ListWidgetHistory;
};
//-----------------------------------------------------------------------------
#endif
