#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISSelectListForm : public ISListBaseForm
{
	Q_OBJECT

signals:
	void Select();

public:
	ISSelectListForm(ISNamespace::SelectListMode select_mode, const QString &table_name, QWidget *parent = 0);
	virtual ~ISSelectListForm();

protected:
	void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected) override;
	void DoubleClickedTable(const QModelIndex &ModelIndex) override;
	void SelectAll();

private:
	QAction *ActionSelect;
	ISNamespace::SelectListMode SelectMode;
};
//-----------------------------------------------------------------------------
