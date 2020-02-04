#include "ISListViewWidget.h"
#include "ISDefines.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISMetaViewQuery.h"
#include "ISGui.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISListViewWidget::ISListViewWidget(PMetaClassTable *meta_query, QWidget *parent) : ISInterfaceMetaForm(parent)
{
	MetaViewQuery = new ISMetaViewQuery(meta_query->GetName());
	MetaQuery = meta_query;

	SqlModel = new ISSqlModelView(MetaQuery, this);

	ModelThreadQuery = new ISModelThreadQuery(this);
	connect(ModelThreadQuery, &ISModelThreadQuery::Started, this, &ISListViewWidget::ModelThreadStarted);
	connect(ModelThreadQuery, &ISModelThreadQuery::Finished, this, &ISListViewWidget::ModelThreadFinished);
	connect(ModelThreadQuery, &ISModelThreadQuery::ExecutedQuery, this, &ISListViewWidget::ModelThreadLoadingData);
	connect(ModelThreadQuery, &ISModelThreadQuery::Results, SqlModel, &ISSqlModelView::SetRecords);
	connect(ModelThreadQuery, &ISModelThreadQuery::ErrorConnection, this, &ISListViewWidget::ModelThreadErrorConnection);
	connect(ModelThreadQuery, &ISModelThreadQuery::ErrorQuery, this, &ISListViewWidget::ModelThreadErrorQuery);
	ModelThreadQuery->start(QThread::TimeCriticalPriority); //Запуск потока

	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetMainLayout()->addWidget(ToolBar);

	ActionUpdate = ISControls::CreateActionUpdate(ToolBar);
	connect(ActionUpdate, &QAction::triggered, this, &ISListViewWidget::Update);
	ToolBar->addAction(ActionUpdate);
	
	LabelAnimation = new QLabel(ToolBar);
	ToolBar->addWidget(LabelAnimation);

	LabelLoading = new QLabel(ToolBar);
	ToolBar->addWidget(LabelLoading);

	TableView = new ISBaseTableView(this);
	TableView->setModel(SqlModel);
	connect(TableView, &ISBaseTableView::doubleClicked, this, &ISListViewWidget::DoubleClicked);
	connect(TableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ISListViewWidget::SelectedRowEvent);
	connect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISListViewWidget::SortingChanged);
	GetMainLayout()->addWidget(TableView);

	LabelRows = new QLabel(this);
	LabelRows->setText(QString("%1: %2...").arg(LANG("RecordsCount")).arg(LANG("Calculated"))); //Изменение значения в надписе "Записей"
	GetMainLayout()->addWidget(LabelRows);
}
//-----------------------------------------------------------------------------
ISListViewWidget::~ISListViewWidget()
{
	ModelThreadQuery->quit();
	IS_ASSERT(ModelThreadQuery->wait(), "Not wait() thread");
	delete MetaViewQuery;
}
//-----------------------------------------------------------------------------
void ISListViewWidget::LoadData()
{
	Update();
}
//-----------------------------------------------------------------------------
int ISListViewWidget::GetCurrentRowIndex() const
{
	int CurrentRow = TableView->selectionModel()->currentIndex().row();
	if (CurrentRow == -1)
	{
		return 0;
	}

	return CurrentRow;
}
//-----------------------------------------------------------------------------
ISSqlModelView* ISListViewWidget::GetSqlModel()
{
	return SqlModel;
}
//-----------------------------------------------------------------------------
void ISListViewWidget::SetClassFilter(const QString &class_filter)
{
	ClassFilter = class_filter;
	MetaViewQuery->SetClassFilter(class_filter);
}
//-----------------------------------------------------------------------------
void ISListViewWidget::ClearClassFilter()
{
	ClassFilter.clear();
	MetaViewQuery->ClearClassFilter();
}
//-----------------------------------------------------------------------------
void ISListViewWidget::SetVisibleHorizontalHeader(bool Visible)
{
	TableView->horizontalHeader()->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISListViewWidget::AddCondition(const QString &Parameter, const QVariant &Value)
{
	Conditions.insert(Parameter, Value);
}
//-----------------------------------------------------------------------------
QToolBar* ISListViewWidget::GetToolBar()
{
	return ToolBar;
}
//-----------------------------------------------------------------------------
void ISListViewWidget::AddAction(QAction *Action, bool AddingToActionGroup, bool AddingToContextMenu)
{

}
//-----------------------------------------------------------------------------
void ISListViewWidget::Update()
{
	SqlModel->Clear();
	ModelThreadQuery->Execute(MetaViewQuery->GetQueryText(), Conditions);
}
//-----------------------------------------------------------------------------
void ISListViewWidget::LoadDataAfterEvent()
{

}
//-----------------------------------------------------------------------------
void ISListViewWidget::DoubleClicked(const QModelIndex &ModelIndex)
{
	emit DoubleClickedSignal(SqlModel->GetRecord(ModelIndex.row()));
}
//-----------------------------------------------------------------------------
void ISListViewWidget::SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected)
{

}
//-----------------------------------------------------------------------------
void ISListViewWidget::ModelThreadStarted()
{
	ISGui::SetWaitGlobalCursor(true);
	ActionUpdate->setEnabled(false);

	QMovie *Movie = BUFFER_ANIMATION("Load", LabelAnimation);
	LabelAnimation->setMovie(Movie);
	Movie->start();

	LabelLoading->setText(LANG("LoadDataPleceWait") + "...");
}
//-----------------------------------------------------------------------------
void ISListViewWidget::ModelThreadFinished()
{
	ISGui::SetWaitGlobalCursor(false);
	ActionUpdate->setEnabled(true);
	LabelRows->setText(QString("%1: %2").arg(LANG("RecordsCount")).arg(SqlModel->rowCount())); //Изменение значения в надписе "Записей"

	QMovie *Movie = LabelAnimation->movie();
	Movie->stop();

	delete Movie;
	Movie = nullptr;

	LabelLoading->setText(QString());
	ISGui::RepaintWidget(ToolBar);

	LoadDataAfterEvent();
}
//-----------------------------------------------------------------------------
void ISListViewWidget::ModelThreadLoadingData()
{
	
}
//-----------------------------------------------------------------------------
void ISListViewWidget::ModelThreadErrorConnection(const QSqlError &SqlError)
{

}
//-----------------------------------------------------------------------------
void ISListViewWidget::ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText)
{

}
//-----------------------------------------------------------------------------
void ISListViewWidget::SortingChanged(int LogicalIndex, Qt::SortOrder SortOrder)
{
	SqlModel->SetCurrentSorting(LogicalIndex, SortOrder);

	MetaViewQuery->SetOrderField(MetaQuery->GetFields().at(LogicalIndex)->GetQueryText());
	MetaViewQuery->SetOrderSort(SortOrder);

	Update();

	//Отсоединение и соединение нужно обязательно, иначе в этом методе произойдет рекурсия
	disconnect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISListViewWidget::SortingChanged);
	TableView->horizontalHeader()->setSortIndicator(LogicalIndex, SortOrder);
	connect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISListViewWidget::SortingChanged);
}
//-----------------------------------------------------------------------------
