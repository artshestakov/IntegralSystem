#include "ISSelectListForm.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISSelectListForm::ISSelectListForm(ISNamespace::SelectListMode select_mode, const QString &table_name, QWidget *parent) : ISListBaseForm(table_name, parent)
{
	SelectMode = select_mode;
	GetToolBar()->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetTableView()->setContextMenuPolicy(Qt::NoContextMenu);

	for (QAction *Action : GetToolBar()->actions())
	{
		Action->setVisible(false);
	}

	ActionSelect = new QAction(this);
	ActionSelect->setText(LANG("Select"));
	ActionSelect->setEnabled(false);
	ActionSelect->setIcon(BUFFER_ICONS("Select"));
	connect(ActionSelect, &QAction::triggered, this, &ISSelectListForm::Select);
	GetToolBar()->addAction(ActionSelect);

	if (SelectMode == ISNamespace::SLM_Single)
	{
		GetTableView()->setSelectionMode(QAbstractItemView::SingleSelection);
	}
	else if (SelectMode == ISNamespace::SLM_Multi)
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
void ISSelectListForm::SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected)
{
	ISListBaseForm::SelectedRowEvent(ItemSelected, ItemDeSelected);

	int SelectedRows = GetTableView()->selectionModel()->selectedRows().count();
	if (SelectedRows)
	{
		ActionSelect->setEnabled(true);
	}
	else
	{
		ActionSelect->setEnabled(false);
	}

	if (SelectMode == ISNamespace::SLM_Multi)
	{
		if (SelectedRows)
		{
			ActionSelect->setText(LANG("Select") + " (" + QString::number(SelectedRows) + ')');
		}
		else
		{
			ActionSelect->setText(LANG("Select"));
		}
	}
}
//-----------------------------------------------------------------------------
void ISSelectListForm::DoubleClickedTable(const QModelIndex &ModelIndex)
{
	emit Select();
}
//-----------------------------------------------------------------------------
void ISSelectListForm::SelectAll()
{
	CornerButtonClicked();
	emit Select();
}
//-----------------------------------------------------------------------------
