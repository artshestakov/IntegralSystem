#include "ISSelectListForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISSelectListForm::ISSelectListForm(ISNamespace::SelectListMode select_mode, const QString &table_name, QWidget *parent)
    : ISListBaseForm(table_name, parent),
    SelectMode(select_mode)
{
    GetToolBar()->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    GetTableView()->setContextMenuPolicy(Qt::NoContextMenu);

    ActionSelect = new QAction(this);
    ActionSelect->setText(LANG("Select"));
    ActionSelect->setEnabled(false);
    ActionSelect->setIcon(BUFFER_ICONS("Select"));
    connect(ActionSelect, &QAction::triggered, this, &ISSelectListForm::Select);
    GetToolBar()->addAction(ActionSelect);

    if (SelectMode == ISNamespace::SelectListMode::Single)
    {
        GetTableView()->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if (SelectMode == ISNamespace::SelectListMode::Multi)
    {
        GetTableView()->setSelectionMode(QAbstractItemView::ExtendedSelection);

        QAction *ActionSelectAll = new QAction(this);
        ActionSelectAll->setText(LANG("SelectAll"));
        ActionSelectAll->setIcon(BUFFER_ICONS("Select"));
        connect(ActionSelectAll, &QAction::triggered, this, &ISSelectListForm::SelectAll);
        GetToolBar()->addAction(ActionSelectAll);
    }
}
//-----------------------------------------------------------------------------
ISSelectListForm::~ISSelectListForm()
{

}
//-----------------------------------------------------------------------------
void ISSelectListForm::AfterShowEvent()
{
    ISListBaseForm::AfterShowEvent();
    for (QAction *Action : GetToolBar()->actions())
    {
        Action->setVisible(false);
    }
    ActionSelect->setVisible(true);
}
//-----------------------------------------------------------------------------
void ISSelectListForm::SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected)
{
    ISListBaseForm::SelectedRowEvent(ItemSelected, ItemDeSelected);

    int SelectedRows = GetTableView()->selectionModel()->selectedRows().count();
    ActionSelect->setEnabled(SelectedRows > 0);

    if (SelectMode == ISNamespace::SelectListMode::Multi)
    {
        ActionSelect->setText(SelectedRows > 0 ? LANG("Select") : LANG_FMT("SelectN", SelectedRows));
    }
}
//-----------------------------------------------------------------------------
void ISSelectListForm::DoubleClickedTable(const QModelIndex &ModelIndex)
{
    Q_UNUSED(ModelIndex);
    emit Select();
}
//-----------------------------------------------------------------------------
void ISSelectListForm::SelectAll()
{
    CornerButtonClicked();
    emit Select();
}
//-----------------------------------------------------------------------------
