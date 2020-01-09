#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceMetaForm.h"
#include "PMetaClassTable.h"
#include "ISBaseTableView.h"
#include "ISModelThreadQuery.h"
#include "ISSqlModelView.h"
#include "ISMetaViewQuery.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISListViewWidget : public ISInterfaceMetaForm
{
	Q_OBJECT

signals:
	void DoubleClickedSignal(const QSqlRecord &SqlRecord); //Сигнал двоного нажатия на строку

public:
	ISListViewWidget(PMetaClassTable *meta_query, QWidget *parent = 0);
	virtual ~ISListViewWidget();

	void LoadData() override;

	int GetCurrentRowIndex() const; //Получить идентификатор текущей строки
	ISSqlModelView* GetSqlModel();

	void SetClassFilter(const QString &class_filter);
	void ClearClassFilter();

	void SetVisibleHorizontalHeader(bool Visible);

	void AddCondition(const QString &Parameter, const QVariant &Value);

protected:
	QToolBar* GetToolBar();
	void AddAction(QAction *Action, bool AddingToActionGroup = true, bool AddingToContextMenu = false);
	
	virtual void Update();
	virtual void LoadDataAfterEvent();
	virtual void DoubleClicked(const QModelIndex &ModelIndex); //Событие двойного нажатия на строку
	virtual void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected); //Событие выбора строки в таблице

	void ModelThreadStarted();
	void ModelThreadFinished();
	void ModelThreadLoadingData();
	void ModelThreadErrorConnection(const QSqlError &SqlError);
	void ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText);

	void SortingChanged(int LogicalIndex, Qt::SortOrder SortOrder);

private:
	QToolBar *ToolBar;
	QAction *ActionUpdate;
	QLabel *LabelAnimation;
	QLabel *LabelLoading;
	ISBaseTableView *TableView;
	QLabel *LabelRows;

	ISMetaViewQuery *MetaViewQuery;
	PMetaClassTable *MetaQuery;
	ISModelThreadQuery *ModelThreadQuery;
	ISSqlModelView *SqlModel;
	QString ClassFilter;
	QVariantMap Conditions;
};
//-----------------------------------------------------------------------------
