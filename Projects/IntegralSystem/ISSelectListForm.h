#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISSelectListForm : public ISListBaseForm
{
    Q_OBJECT

signals:
    void Select();

public:
    ISSelectListForm(ISNamespace::SelectListMode select_mode, const QString &table_name, QWidget *parent = 0);
    virtual ~ISSelectListForm();

protected:
    void AfterShowEvent() override;
    void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected) override;
    void DoubleClickedTable(const QModelIndex &ModelIndex) override;
    void SelectAll();

private:
    QAction *ActionSelect;
    ISNamespace::SelectListMode SelectMode;
};
//-----------------------------------------------------------------------------
