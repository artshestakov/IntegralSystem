#include "ISListBaseForm.h"
#include "ISDefinesGui.h"
#include "ISAssert.h"
#include "ISCore.h"
#include "ISSettings.h"
#include "ISLocalization.h"
#include "ISSortingBuffer.h"
#include "ISProtocol.h"
#include "ISPopupMessage.h"
#include "ISExportForm.h"
#include "ISProgressForm.h"
#include "ISLogger.h"
#include "ISProcessForm.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISMessageBox.h"
#include "ISPrintingBase.h"
#include "ISPrintingHtml.h"
#include "ISUserRoleEntity.h"
#include "ISSystem.h"
#include "ISQuery.h"
#include "ISPrintingEntity.h"
#include "ISPrintForm.h"
#include "ISDatabaseHelper.h"
#include "ISDelegates.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISMetaUser.h"
#include "ISColumnSizer.h"
#include "ISInputDialog.h"
#include "ISExportWorker.h"
#include "ISExportCSV.h"
#include "ISExportHTML.h"
#include "ISExportDBF.h"
#include "ISExportXML.h"
#include "ISQueryPool.h"
#include "ISGui.h"
#include "ISAlgorithm.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
static QString QI_SEARCH_FAST = PREPARE_QUERY("INSERT INTO _searchfast(srfs_user, srfs_value) "
											  "VALUES(:UserID, :Value)");
//-----------------------------------------------------------------------------
ISListBaseForm::ISListBaseForm(const QString &TableName, QWidget *parent)
	: ISInterfaceMetaForm(parent),
	ActionObjectGroup(new QActionGroup(this)), //������ ��������, ����������� ������ � ������ �������
	MetaTable(ISMetaData::Instance().GetMetaTable(TableName)),
	SqlModel(nullptr),
	PageNavigation(nullptr),
	QueryModel(nullptr),
	SearchForm(nullptr),
	SelectObjectAfterUpdate(0),
	DelegatesCreated(false),
	IsLoadingData(false),
	SearchFlag(false)
{
	{//�������� ��������
		//�������
		QAction *ActionCreate = ISControls::CreateActionCreate(this);
		ActionCreate->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		connect(ActionCreate, &QAction::triggered, this, &ISListBaseForm::Create);
		Actions[ISNamespace::AT_Create] = ActionCreate;

		//������� �����
		QAction *ActionCreateCopy = ISControls::CreateActionCreateCopy(this);
		connect(ActionCreateCopy, &QAction::triggered, this, &ISListBaseForm::CreateCopy);
		Actions[ISNamespace::AT_CreateCopy] = ActionCreateCopy;

		//��������
		QAction *ActionEdit = ISControls::CreateActionEdit(this);
		connect(ActionEdit, &QAction::triggered, this, &ISListBaseForm::Edit);
		Actions[ISNamespace::AT_Edit] = ActionEdit;

		//�������
		QAction *ActionDelete = ISControls::CreateActionDelete(this);
		connect(ActionDelete, &QAction::triggered, this, &ISListBaseForm::Delete);
		Actions[ISNamespace::AT_Delete] = ActionDelete;

		//��������
		QAction *ActionUpdate = ISControls::CreateActionUpdate(this);
		connect(ActionUpdate, &QAction::triggered, this, &ISListBaseForm::Update);
		Actions[ISNamespace::AT_Update] = ActionUpdate;

		//�����
		QAction *ActionSearch = ISControls::CreateActionSearch(this);
		connect(ActionSearch, &QAction::triggered, this, &ISListBaseForm::Search);
		Actions[ISNamespace::AT_Search] = ActionSearch;

		//������� ����������� ������
		QAction *ActionSearchClearResult = ISControls::CreateActionSearchClearResults(this);
		ActionSearchClearResult->setEnabled(false);
		connect(ActionSearchClearResult, &QAction::triggered, this, &ISListBaseForm::SearchClear);
		Actions[ISNamespace::AT_SearchClear] = ActionSearchClearResult;

		//�������
		QAction *ActionExport = new QAction(BUFFER_ICONS("ExportTable"), LANG("ListForm.ExportTable"), this);
		ActionExport->setShortcut(QKeySequence(Qt::Key_F12));
		connect(ActionExport, &QAction::triggered, this, &ISListBaseForm::Export);
		Actions[ISNamespace::AT_Export] = ActionExport;

		//������
		QAction *ActionPrint = ISControls::CreateActionPrint(this);
		ActionPrint->setVisible(ISPrintingEntity::Instance().GetCountReports(MetaTable->Name));
		connect(ActionPrint, &QAction::triggered, this, &ISListBaseForm::Print);
		Actions[ISNamespace::AT_Print] = ActionPrint;

		//���������
		QAction *ActionFavorites = new QAction(BUFFER_ICONS("Favorites"), LANG("Favorites"), this);
		connect(ActionFavorites, &QAction::triggered, this, &ISListBaseForm::ShowFavorites);
		Actions[ISNamespace::AT_Favorites] = ActionFavorites;

		//������ ������
		QAction *ActionNavigationBegin = new QAction(BUFFER_ICONS("TableNavigationBegin"), LANG("TableNavigationSelectBegin"), this);
		ActionNavigationBegin->setShortcut(QKeySequence(Qt::Key_Home));
		connect(ActionNavigationBegin, &QAction::triggered, this, &ISListBaseForm::NavigationSelectBeginRecord);
		Actions[ISNamespace::AT_NavigationBegin] = ActionNavigationBegin;

		//���������� ������
		QAction *ActionNavigationPrevious = new QAction(BUFFER_ICONS("TableNavigationPrevious"), LANG("TableNavigationSelectPrevious"), this);
		connect(ActionNavigationPrevious, &QAction::triggered, this, &ISListBaseForm::NavigationSelectPreviousRecord);
		Actions[ISNamespace::AT_NavigationPrevious] = ActionNavigationPrevious;

		//��������� ������
		QAction *ActionNavigationNext = new QAction(BUFFER_ICONS("TableNavigationNext"), LANG("TableNavigationSelectNext"), this);
		connect(ActionNavigationNext, &QAction::triggered, this, &ISListBaseForm::NavigationSelectNextRecord);
		Actions[ISNamespace::AT_NavigationNext] = ActionNavigationNext;

		//��������� ������
		QAction *ActionNavigationLast = new QAction(BUFFER_ICONS("TableNavigationLast"), LANG("TableNavigationSelectLast"), this);
		ActionNavigationLast->setShortcut(QKeySequence(Qt::Key_End));
		connect(ActionNavigationLast, &QAction::triggered, this, &ISListBaseForm::NavigationSelectLastRecord);
		Actions[ISNamespace::AT_NavigationLast] = ActionNavigationLast;
	}

	{//�������� ����������� ��������
		//���������� �� ���������
		QAction *ActionSortDefault = new QAction(BUFFER_ICONS("DefaultSorting"), LANG("DefaultSorting"), this);
		connect(ActionSortDefault, &QAction::triggered, this, &ISListBaseForm::SortingDefault);
		ActionsSpecial.emplace(ISNamespace::AST_SortDefault, ActionSortDefault);

		//����������
		QAction *ActionNoteObject = ISControls::CreateActionNoteObject(this);
		connect(ActionNoteObject, &QAction::triggered, this, &ISListBaseForm::NoteObject);
		ActionsSpecial.emplace(ISNamespace::AST_Note, ActionNoteObject);

		//���������� ������
		QAction *ActionResizeFromContent = new QAction(this);
		ActionResizeFromContent->setText(LANG("AutoFitColumnWidth"));
		ActionResizeFromContent->setToolTip(LANG("AutoFitColumnWidth"));
		ActionResizeFromContent->setIcon(BUFFER_ICONS("AutoFitColumnWidth"));
		connect(ActionResizeFromContent, &QAction::triggered, this, &ISListBaseForm::AutoFitColumnWidth);
		ActionsSpecial.emplace(ISNamespace::AST_ResizeFromContent, ActionResizeFromContent);

		//����� ������ �������
		QAction *ActionResetWidthColumn = new QAction(this);
		ActionResetWidthColumn->setText(LANG("ResetWidthColumn"));
		ActionResetWidthColumn->setToolTip(LANG("ResetWidthColumn"));
		connect(ActionResetWidthColumn, &QAction::triggered, this, &ISListBaseForm::ResetWidthColumn);
		ActionsSpecial.emplace(ISNamespace::AST_ResetWidthColumn, ActionResetWidthColumn);

		ActionObjectGroup->addAction(ActionNoteObject);
	}

	{//�������� �������
		ToolBar = new QToolBar(this);
		ToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		ToolBar->setEnabled(false);
		GetMainLayout()->addWidget(ToolBar);

		ToolBar->addAction(GetAction(ISNamespace::AT_Create));
		ToolBar->addAction(GetAction(ISNamespace::AT_CreateCopy));
		ToolBar->addAction(GetAction(ISNamespace::AT_Edit));
		ToolBar->addAction(GetAction(ISNamespace::AT_Delete));
		ToolBar->addAction(GetAction(ISNamespace::AT_Update));
		ToolBar->addAction(GetAction(ISNamespace::AT_Search));
		ToolBar->addAction(GetAction(ISNamespace::AT_SearchClear));
		ToolBar->addAction(GetAction(ISNamespace::AT_Print));

		QAction *ActionAdditionally = ToolBar->addAction(BUFFER_ICONS("AdditionallyActions"), LANG("Additionally"));
		dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionAdditionally))->setPopupMode(QToolButton::InstantPopup);
		dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionAdditionally))->setStyleSheet(BUFFER_STYLE_SHEET("QToolButtonMenu"));
		ActionAdditionally->setMenu(new QMenu(ToolBar));
		ActionAdditionally->menu()->addAction(GetAction(ISNamespace::AT_Favorites));
		ActionAdditionally->menu()->addAction(GetAction(ISNamespace::AT_Export));
		ActionAdditionally->menu()->addAction(GetSpecialAction(ISNamespace::AST_SortDefault));
		ActionAdditionally->menu()->addAction(GetSpecialAction(ISNamespace::AST_ResizeFromContent));
		ActionAdditionally->menu()->addAction(GetSpecialAction(ISNamespace::AST_ResetWidthColumn));
		ActionAdditionally->menu()->addAction(LANG("SettingsList"), this, &ISListBaseForm::ShowSettingsForm);

		if (SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWNAVIGATION))
		{
			ToolBar->addAction(GetAction(ISNamespace::AT_NavigationBegin));
			ToolBar->addAction(GetAction(ISNamespace::AT_NavigationPrevious));
			ToolBar->addAction(GetAction(ISNamespace::AT_NavigationNext));
			ToolBar->addAction(GetAction(ISNamespace::AT_NavigationLast));
		}

		if (GetAction(ISNamespace::AT_CreateCopy)) ActionObjectGroup->addAction(GetAction(ISNamespace::AT_CreateCopy));
		if (GetAction(ISNamespace::AT_Edit)) ActionObjectGroup->addAction(GetAction(ISNamespace::AT_Edit));
		if (GetAction(ISNamespace::AT_Delete)) ActionObjectGroup->addAction(GetAction(ISNamespace::AT_Delete));
		if (GetAction(ISNamespace::AT_Print)) ActionObjectGroup->addAction(GetAction(ISNamespace::AT_Print));
	}

	{//�������� �������
		TableView = new ISBaseTableView(this);
		TableView->SetCornerText(LANG("Reduction.SerialNumber"));
		TableView->SetCornerToolTip(LANG("OrdinalNumber"));
		TableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
		connect(TableView, &ISBaseTableView::doubleClicked, this, &ISListBaseForm::DoubleClickedTable);
		connect(TableView, &ISBaseTableView::customContextMenuRequested, this, &ISListBaseForm::ShowContextMenu);
		connect(TableView, &ISBaseTableView::CornerClicked, this, &ISListBaseForm::CornerButtonClicked);
		connect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISListBaseForm::SortingChanged);
		FieldResized(true);
		GetMainLayout()->addWidget(TableView);

		if (SETTING_BOOL(CONST_UID_SETTING_TABLES_MINIMIZEHEIGHTROWS))
		{
			TableView->verticalHeader()->setDefaultSectionSize(19);
		}
		TableView->horizontalHeader()->setVisible(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWHORIZONTALHEADER));
		TableView->SetVisibleVerticalHeader(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWVERTICALHEADER));
	}

	{//�������� ������������ ����
		ContextMenu = new QMenu(this);
		if (GetAction(ISNamespace::AT_Create)) ContextMenu->addAction(GetAction(ISNamespace::AT_Create));
		if (GetAction(ISNamespace::AT_CreateCopy)) ContextMenu->addAction(GetAction(ISNamespace::AT_CreateCopy));
		if (GetAction(ISNamespace::AT_Edit)) ContextMenu->addAction(GetAction(ISNamespace::AT_Edit));
		if (GetAction(ISNamespace::AT_Delete)) ContextMenu->addAction(GetAction(ISNamespace::AT_Delete));
		if (GetAction(ISNamespace::AT_Update)) ContextMenu->addAction(GetAction(ISNamespace::AT_Update));
		ContextMenu->addAction(GetSpecialAction(ISNamespace::AST_Note));
	}

	{//�������� �������
		SqlModel = new ISSqlModelCore(MetaTable, TableView);
		TableView->setModel(SqlModel);

		QueryModel = new ISQueryModel(MetaTable, ISNamespace::QMT_List, this);

		ISSortingMetaTable *MetaSorting = ISSortingBuffer::Instance().GetSorting(MetaTable->Name);
		if (MetaSorting) //���� ���������� ��� ���� ������� ��� ����������, ������������ �
		{
			QueryModel->SetSorting(MetaSorting->FieldName, MetaSorting->Order);
		}

		//��� ���������� ����������� ������ ���� ����� ������������ ������ � QTableView
		connect(TableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ISListBaseForm::SelectedRowEvent);

		//�������� ��������� ������
		ModelThreadQuery = new ISModelThreadQuery(this);
		connect(ModelThreadQuery, &ISModelThreadQuery::Started, this, &ISListBaseForm::ModelThreadStarted);
		connect(ModelThreadQuery, &ISModelThreadQuery::Finished, this, &ISListBaseForm::ModelThreadFinished);
		connect(ModelThreadQuery, &ISModelThreadQuery::ExecutedQuery, this, &ISListBaseForm::ModelThreadLoadingData);
		connect(ModelThreadQuery, &ISModelThreadQuery::Results, SqlModel, &ISSqlModelCore::SetRecords);
		connect(ModelThreadQuery, &ISModelThreadQuery::ErrorConnection, this, &ISListBaseForm::ModelThreadErrorConnection);
		connect(ModelThreadQuery, &ISModelThreadQuery::ErrorQuery, this, &ISListBaseForm::ModelThreadErrorQuery);
		ModelThreadQuery->start(QThread::TimeCriticalPriority); //������ ������
	}
	
	{//�������� ������-����
		StatusBar = new QStatusBar(this);
		StatusBar->setSizeGripEnabled(false);
		GetMainLayout()->addWidget(StatusBar);

		LabelRowCount = new QLabel(LANG("RecordsCount") + ": -", StatusBar);
		LabelRowCount->setVisible(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWCOUNTRECORD));
		StatusBar->addWidget(LabelRowCount);

		if (SETTING_BOOL(CONST_UID_SETTING_TABLES_PAGE_NAVIGATION))
		{
			QueryModel->SetLimit(SETTING_INT(CONST_UID_SETTING_TABLES_PAGE_NAVIGATION_LIMIT));

			PageNavigation = new ISPageNavigation(StatusBar);
			PageNavigation->SetLimit(SETTING_INT(CONST_UID_SETTING_TABLES_PAGE_NAVIGATION_LIMIT));
			connect(PageNavigation, &ISPageNavigation::OffsetSignal, QueryModel, &ISQueryModel::SetOffset);
			connect(PageNavigation, &ISPageNavigation::Update, this, &ISListBaseForm::Update);
			StatusBar->addWidget(PageNavigation);
		}

		LabelSelectedRow = new QLabel(StatusBar);
		LabelSelectedRow->setVisible(false);
		StatusBar->addWidget(LabelSelectedRow);

		EditSearch = new ISSearchEdit(StatusBar);
		EditSearch->setSizePolicy(QSizePolicy::Maximum, EditSearch->sizePolicy().verticalPolicy());
		connect(EditSearch, &ISSearchEdit::Search, this, &ISListBaseForm::SearchFast);
		connect(EditSearch, &ISSearchEdit::ClearPressed, this, &ISListBaseForm::SearchFastClear);
		connect(this, &ISListBaseForm::Updated, EditSearch, &ISSearchEdit::Updated);
		connect(this, &ISListBaseForm::Updated, EditSearch, static_cast<void(ISSearchEdit::*)(void)>(&ISSearchEdit::setFocus));
		StatusBar->addPermanentWidget(EditSearch);
	}

	//�������� ����� ������� ������ ����������� ����� �������� ���� ���������
	ListIndicatorWidget = new ISListIndicatorWidget(this);
}
//-----------------------------------------------------------------------------
ISListBaseForm::~ISListBaseForm()
{
	ModelThreadQuery->quit();
	IS_ASSERT(ModelThreadQuery->wait(), "Not wait() thread");

	if (SearchForm)
	{
		delete SearchForm;
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
int ISListBaseForm::GetCurrentRowIndex()
{
	return TableView->selectionModel()->currentIndex().row();
}
//-----------------------------------------------------------------------------
int ISListBaseForm::GetObjectID()
{
	int ObjectID = GetCurrentRecordValue("ID").toInt();
	if (!ObjectID)
	{
		IS_ASSERT(false, "ObjectID invalid.");
	}
	return ObjectID;
}
//-----------------------------------------------------------------------------
int ISListBaseForm::GetObjectID(int RowIndex)
{
	return SqlModel->index(RowIndex, 0).data().toInt();
}
//-----------------------------------------------------------------------------
int ISListBaseForm::GetRowIndex(int object_id)
{
	for (int i = 0; i < SqlModel->rowCount(); ++i)
	{
		int RowIndex = SqlModel->data(SqlModel->index(i, 0)).toInt();
		if (RowIndex == object_id)
		{
			return i;
		}
	}
	return -1;
}
//-----------------------------------------------------------------------------
QSqlRecord ISListBaseForm::GetCurrentRecord()
{
	return SqlModel->GetRecord(GetCurrentRowIndex());
}
//-----------------------------------------------------------------------------
QVariant ISListBaseForm::GetCurrentRecordValue(const QString &FieldName)
{
	return GetCurrentRecord().value(FieldName);
}
//-----------------------------------------------------------------------------
QVariant ISListBaseForm::GetCurrentRecordValueDB(const QString &FieldName)
{
	QVariant Value;
	ISQuery qSelect(QString("SELECT %1_%2 FROM %3 WHERE %1_id = :ObjectID").arg(MetaTable->Alias).arg(FieldName.toLower()).arg(MetaTable->Name.toLower()));
	qSelect.BindValue(":ObjectID", GetObjectID());
	if (qSelect.ExecuteFirst())
	{
		Value = qSelect.ReadColumn(0);
	}
	return Value;
}
//-----------------------------------------------------------------------------
QVariant ISListBaseForm::GetRecordValue(const QString &FieldName, int RowIndex)
{
	return SqlModel->GetRecord(RowIndex).value(FieldName);
}
//-----------------------------------------------------------------------------
ISVectorInt ISListBaseForm::GetSelectedIDs()
{
	ISVectorInt VectorInt;
	QModelIndexList ModelIndexList = GetTableView()->selectionModel()->selectedRows();
	if (!ModelIndexList.isEmpty())
	{
		for (const QModelIndex &ModelIndex : ModelIndexList)
		{
			VectorInt.emplace_back(GetObjectID(ModelIndex.row()));
		}
	}
	return VectorInt;
}
//-----------------------------------------------------------------------------
int ISListBaseForm::GetCountSelected()
{
	return GetTableView()->selectionModel()->selectedRows().count();
}
//-----------------------------------------------------------------------------
ISVectorInt ISListBaseForm::GetIDs() const
{
	ISVectorInt VectorInt;
	for (int i = 0; i < SqlModel->rowCount(); ++i)
	{
		VectorInt.emplace_back(SqlModel->data(SqlModel->index(i, SqlModel->GetFieldIndex("ID"))).toInt());
	}
	return VectorInt;
}
//-----------------------------------------------------------------------------
ISVectorInt ISListBaseForm::GetSelectedRowIndexes()
{
	ISVectorInt VectorInt;
	QModelIndexList ModelIndexList = GetTableView()->selectionModel()->selectedRows();
	if (ModelIndexList.count())
	{
		for (const QModelIndex &ModelIndex : ModelIndexList)
		{
			VectorInt.emplace_back(ModelIndex.row());
		}
	}
	return VectorInt;
}
//-----------------------------------------------------------------------------
ISQueryModel* ISListBaseForm::GetQueryModel()
{
	return QueryModel;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SetSelectObjectAfterUpdate(int ObjectID)
{
	SetSelectObjectAfterUpdate(ISVectorInt{ ObjectID });
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SetSelectObjectAfterUpdate(const ISVectorInt &Objects)
{
	SelectObjectAfterUpdate = Objects;
}
//-----------------------------------------------------------------------------
PMetaTable* ISListBaseForm::GetMetaTable()
{
	return MetaTable;
}
//-----------------------------------------------------------------------------
ISSqlModelCore* ISListBaseForm::GetSqlModel()
{
	return SqlModel;
}
//-----------------------------------------------------------------------------
QStatusBar* ISListBaseForm::GetStatusBar()
{
	return StatusBar;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::DoubleClickedTable(const QModelIndex &ModelIndex)
{
	Q_UNUSED(ModelIndex);
	if (MetaTable->ShowOnly)
	{
		return;
	}
	
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	QString EventName = SETTING_STRING(CONST_UID_SETTING_TABLES_DOUBLECLICKEVENT);
	if (EventName == "Window")
	{
		ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, MetaTable->Name, GetObjectID());
		ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
		connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
		connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
		connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
		connect(ObjectFormBase, &ISObjectFormBase::Close, TableView, static_cast<void(ISBaseTableView::*)(void)>(&ISBaseTableView::setFocus));
		ISGui::ShowObjectForm(ObjectFormBase);
	}
	else if (EventName == "Tab")
	{
		Edit();
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::FieldResized(bool Include)
{
	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERCOLUMNSIZE))
	{
		Include ?
			connect(TableView->horizontalHeader(), &QHeaderView::sectionResized, this, static_cast<void(ISListBaseForm::*)(int, int, int)>(&ISListBaseForm::FieldResized)) :
			disconnect(TableView->horizontalHeader(), &QHeaderView::sectionResized, this, static_cast<void(ISListBaseForm::*)(int, int, int)>(&ISListBaseForm::FieldResized));
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::FieldResized(int LogicalIndex, int WidthOld, int WidthNew)
{
	Q_UNUSED(WidthOld);
	ISColumnSizer::Instance().SetColumnSize(MetaTable->Name, SqlModel->headerData(LogicalIndex, Qt::Horizontal, Qt::UserRole).toString(), WidthNew);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SortingChanged(int LogicalIndex, Qt::SortOrder Order)
{
	if (Order == SqlModel->GetSortOrder() && LogicalIndex == SqlModel->GetSortColumn())
	{
		Order = Order == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
	}

	QString FieldName = SqlModel->headerData(LogicalIndex, Qt::Horizontal, Qt::UserRole).toString();
	QueryModel->SetSorting(FieldName, Order);
	SqlModel->SetSorting(LogicalIndex, Order);

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERSORTING))
	{
		ISSortingBuffer::Instance().AddSorting(MetaTable->Name, FieldName, Order);
	}

	Update();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SortingDefault()
{
	SortingChanged(0, Qt::AscendingOrder);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::VisibleIndicatorWidget()
{
	bool is_visible = !SqlModel->rowCount();
	ListIndicatorWidget->setVisible(is_visible);
	ListIndicatorWidget->SetPixmap(is_visible ? BUFFER_ICONS("LabelNoDataTable").pixmap(ISDefines::Gui::SIZE_32_32) : QPixmap());
	ListIndicatorWidget->SetText(is_visible ? LANG("NoData") : QString());
	ListIndicatorWidget->setCursor(is_visible ? CURSOR_WHATS_THIS : CURSOR_WAIT);
	ListIndicatorWidget->setToolTip(is_visible ? LANG("ClickCreateFromCreateObject") : QString());
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowSettingsForm()
{
	ISGui::ShowSettingsForm(CONST_UID_SETTING_GROUP_TABLES);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected)
{
	Q_UNUSED(ItemSelected);
	Q_UNUSED(ItemDeSelected);

	int SelectedRows = TableView->selectionModel()->selectedRows().count();
	if (SelectedRows > 1 || SelectedRows == 0)
	{
		SetEnabledActionObject(false);
	}
	else
	{
		SetEnabledActionObject(true);
	}

	if (SelectedRows)
	{
		LabelSelectedRow->setVisible(true);
		LabelSelectedRow->setText(LANG("SelectedRecords") + ": " + QString::number(SelectedRows));
	}
	else
	{
		LabelSelectedRow->setVisible(false);
		LabelSelectedRow->clear();
	}

	GetAction(ISNamespace::AT_Delete)->setEnabled(SelectedRows);
	emit SelectedRowSignal();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::LoadData()
{
	Update();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CornerButtonClicked()
{
	if (SqlModel->rowCount())
	{
		ISGui::SetWaitGlobalCursor(true);
		TableView->selectAll();
		TableView->setFocus();
		ISGui::SetWaitGlobalCursor(false);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::LoadDataAfterEvent()
{
	ResizeColumnsToContents();
	LabelRowCount->setText(QString("%1: %2").arg(LANG("RecordsCount")).arg(SqlModel->rowCount()));

	SetEnabledActionObject(false);
	SelectRowObject(SelectObjectAfterUpdate);
	VisibleIndicatorWidget();

	//���� ������ � ������ ��� � �������� ���� ����� ������ - ���������� ��������������� ������� � ������ ���� ������
	if (!SqlModel->rowCount() && SearchFlag)
	{
		SearchFlag = false;
		ListIndicatorWidget->SetText(LANG("Search.Results.Empty"));
	}
	emit Updated();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::AfterShowEvent()
{
	ISInterfaceMetaForm::AfterShowEvent();
	GetAction(ISNamespace::AT_Create)->setVisible(!MetaTable->ShowOnly);
	GetAction(ISNamespace::AT_CreateCopy)->setVisible(!MetaTable->ShowOnly);
	GetAction(ISNamespace::AT_Edit)->setVisible(!MetaTable->ShowOnly);
	GetAction(ISNamespace::AT_Delete)->setVisible(!MetaTable->ShowOnly);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::AddAction(QAction *Action, bool AddingToActionGroup, bool AddingToContextMenu)
{
	ToolBar->addAction(Action);
	TableView->addAction(Action);
	if (AddingToActionGroup)
	{
		ActionObjectGroup->addAction(Action);
	}
	if (AddingToContextMenu)
	{
		ContextMenu->addAction(Action);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::InsertAction(QAction *Action, QAction *ActionBefore, bool AddingToActionGroup, bool AddingToContextMenu)
{
	ToolBar->insertAction(ActionBefore, Action);
	TableView->insertAction(ActionBefore, Action);
	if (AddingToActionGroup)
	{
		ActionObjectGroup->addAction(Action);
	}
	if (AddingToContextMenu)
	{
		ContextMenu->insertAction(ActionBefore, Action);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::paintEvent(QPaintEvent *e)
{
	ISInterfaceMetaForm::paintEvent(e);
	if (ListIndicatorWidget->isVisible())
	{
		QPoint CenterPoint = TableView->frameGeometry().center();
		CenterPoint.setX(CenterPoint.x() - (ListIndicatorWidget->width() / 2));
		CenterPoint.setY(CenterPoint.y() - (ListIndicatorWidget->height() / 2));
		ListIndicatorWidget->move(CenterPoint);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::EscapeClicked()
{
	if (!parentWidget())
	{
		close();
	}
}
//-----------------------------------------------------------------------------
ISBaseTableView* ISListBaseForm::GetTableView()
{
	return TableView;
}
//-----------------------------------------------------------------------------
QToolBar* ISListBaseForm::GetToolBar()
{
	return ToolBar;
}
//-----------------------------------------------------------------------------
QAction* ISListBaseForm::GetAction(ISNamespace::ActionType action_type)
{
	return Actions[action_type];
}
//-----------------------------------------------------------------------------
QAction* ISListBaseForm::GetSpecialAction(ISNamespace::ActionSpecialType action_special)
{
	return ActionsSpecial[action_special];
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateDelegates()
{
	if (DelegatesCreated) //���� �������� ��� ������� - �������� �� ������
	{
		return;
	}

	for (int i = 0; i < SqlModel->columnCount(); ++i) //����� �����
	{
		QString HeaderData = SqlModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString();
		PMetaField *MetaField = MetaTable->GetField(HeaderData);

		QAbstractItemDelegate *AbstractItemDelegate = TableView->itemDelegateForColumn(i);
		if (!AbstractItemDelegate)
		{
			switch (MetaField->Type)
			{
			case ISNamespace::FT_Bool: AbstractItemDelegate = new ISDelegateBoolean(TableView); break;
			case ISNamespace::FT_Image: AbstractItemDelegate = new ISDelegateImage(TableView); break;
			case ISNamespace::FT_Color: AbstractItemDelegate = new ISDelegateColor(TableView); break;
			case ISNamespace::FT_File: AbstractItemDelegate = new ISDelegateFile(TableView); break;
			case ISNamespace::FT_TaskImportant: AbstractItemDelegate = new ISDelegateTaskImportant(TableView); break;
			}
			if (AbstractItemDelegate)
			{
				TableView->setItemDelegateForColumn(i, AbstractItemDelegate);
			}
		}
	}
	DelegatesCreated = true;
}
//-----------------------------------------------------------------------------
bool ISListBaseForm::CheckIsSystemObject()
{
	return GetCurrentRecordValue("IsSystem").toBool();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::HideField(const QString &FieldName)
{
	FieldResized(false);
	TableView->hideColumn(SqlModel->GetFieldIndex(FieldName));
	FieldResized(true);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowField(const QString &FieldName)
{
	FieldResized(false);
	TableView->showColumn(SqlModel->GetFieldIndex(FieldName));
	FieldResized(true);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ResizeColumnsToContents()
{
	FieldResized(false);
	for (int i = 0, c = SqlModel->columnCount(); i < c; ++i)
	{
		QString FieldName = SqlModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString();
		int ColumnSize = ISColumnSizer::Instance().GetColumnSize(MetaTable->Name, FieldName);
		if (ColumnSize) //���� ���� ������ ������� � ������, ������������ ���
		{
			TableView->setColumnWidth(i, ColumnSize);
		}
	}
	TableView->resizeColumnToContents(SqlModel->GetFieldIndex("ID"));
	FieldResized(true);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SelectRowObject(const ISVectorInt &Objects)
{
	if (!Objects.empty()) //���� ������ �� ������
	{
		//������������� ����� ��������� �� �������������� (������ ��� ���� ����� �� �������,
		//�� ������� �� ����� � Objects �� ����, �������� ����� ������ ���� ������)
		TableView->setSelectionMode(QAbstractItemView::MultiSelection);

		//������� ��� ������
		for (int ObjectID : Objects)
		{
			int RowIndex = GetRowIndex(ObjectID); //�������� ������ ������ �� � ��������������
			if (RowIndex != -1) //���� ������ �������� - �������� ������
			{
				SelectRowIndex(RowIndex);
			}
		}

		//���������� ����� ��������� �� �������� � ������� ������ ���������
		TableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
		SelectObjectAfterUpdate.clear();
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SelectRowIndex(int row_index)
{
	TableView->selectRow(row_index);
	TableView->scrollTo(TableView->model()->index(row_index, 0));
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SetEnabledActionObject(bool Enabled)
{
	for (int i = 0; i < ActionObjectGroup->actions().count(); ++i)
	{
		ActionObjectGroup->actions().at(i)->setEnabled(Enabled);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SetEnabledPageNavigation(bool Enabled)
{
	if (PageNavigation)
	{
		PageNavigation->setEnabled(Enabled);
		ISGui::RepaintWidget(PageNavigation);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SearchFast(const QString &SearchValue)
{
	ISQueryPool::Instance().AddQuery(QI_SEARCH_FAST,
	{
		{ ":UserID", CURRENT_USER_ID },
		{ ":Value", SearchValue }
	});

	QString PreparedSearchValue = SearchValue.toLower(); //�������������� ���������� ������� � ������ ������� (�����������!!!)
	if (PreparedSearchValue.length()) //���� ������������ ���� ��������� ��������
	{
		EditSearch->setAccessibleName(PreparedSearchValue); //��������� ���������� ��������� ��������
		QString WhereText = "lower(concat(";

		for (int Column = 0; Column < SqlModel->columnCount(); ++Column) //����� ����� ������
		{
			PMetaField *MetaField = SqlModel->GetField(Column);
			if (!ISMetaData::Instance().GetSearch(MetaField->Type))
			{
				continue;
			}

			if (!MetaField->QueryText.isEmpty()) //���� ���� �������� �����������
			{
				WhereText += '(' + MetaField->QueryText + ')';
			}
			else //����������� ����-����
			{
				if (MetaField->Foreign) //���� �� ���� ���������� ������� ����
				{
					PMetaTable *MetaForeignTable = ISMetaData::Instance().GetMetaTable(MetaField->Foreign->ForeignClass);
					WhereText += "(SELECT concat(";
					for (const QString &FieldName : MetaField->Foreign->ForeignViewNameField.split(';'))
					{
						WhereText += MetaForeignTable->Alias + '_' + FieldName + ", ";
					}
					WhereText.chop(2);
					WhereText += ") FROM " + MetaForeignTable->Name + SYMBOL_SPACE;
					WhereText += "WHERE " + MetaForeignTable->Alias + '_' + MetaField->Foreign->ForeignField + " = " + MetaTable->Alias + '_' + MetaField->Name + ')';
				}
				else //���� ��� �������� �����
				{
					WhereText += MetaTable->Alias + '.' + MetaTable->Alias + '_' + MetaField->Name;
				}
			}

			WhereText += ", ';', ";
		}

		WhereText.chop(7);
		WhereText += ")) LIKE '%' || :Search || '%'";

		QueryModel->SetSearchFilter(WhereText);
		QueryModel->ClearConditions();
		QueryModel->AddCondition(":Search", PreparedSearchValue);

		Update();
		SearchFlag = true;
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SearchFastClear()
{
	disconnect(EditSearch, &ISSearchEdit::Search, this, &ISListBaseForm::SearchFast);
	SearchFlag = false;
	QueryModel->ClearSearchFilter();
	QueryModel->ClearConditions();
	Update();
	connect(EditSearch, &ISSearchEdit::Search, this, &ISListBaseForm::SearchFast);
	GetAction(ISNamespace::AT_SearchClear)->setEnabled(false);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadStarted()
{
	ISGui::SetWaitGlobalCursor(true);

	ListIndicatorWidget->SetPixmap(QPixmap());
	ListIndicatorWidget->SetVisibleAnimation(true);
	ListIndicatorWidget->SetText(LANG("LoadDataPleceWait"));
	ListIndicatorWidget->setVisible(true);

	LabelRowCount->setText(QString("%1: %2...").arg(LANG("RecordsCount")).arg(LANG("Calculated"))); //��������� �������� � ������� "�������"
	ToolBar->setEnabled(false);
	SetEnabledPageNavigation(false);
	EditSearch->setEnabled(false);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadLoadingData()
{
	ListIndicatorWidget->SetText(LANG("FillTableData"));
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadFinished()
{
	IsLoadingData = false;
	ISGui::SetWaitGlobalCursor(false);

	ListIndicatorWidget->hide();
	ToolBar->setEnabled(true);
	SetEnabledPageNavigation(true);
	EditSearch->setEnabled(true);

	if (!SETTING_BOOL(CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID))
	{
		HideField("ID");
	}
	HideField("IsSystem");

	ISSortingMetaTable *MetaSorting = ISSortingBuffer::Instance().GetSorting(MetaTable->Name);
	if (MetaSorting) //���� ���������� ��� ���� ������� ��� ����������, ������������ �
	{
		SqlModel->SetSorting(SqlModel->GetFieldIndex(MetaSorting->FieldName), MetaSorting->Order);
	}

	CreateDelegates();
	LoadDataAfterEvent();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadErrorConnection(const QSqlError &SqlError)
{
	ISGui::SetWaitGlobalCursor(false);

	ListIndicatorWidget->hide();
	ToolBar->setEnabled(true);

	ISLOGGER_W(__CLASS__, SqlError.text());
	ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionLoadingData") + "\n\n" + SqlError.text());
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText)
{
	ISGui::SetWaitGlobalCursor(false);

	ListIndicatorWidget->hide();
	ToolBar->setEnabled(true);

	ISLOGGER_W(__CLASS__, SqlError.text());
	ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionLoadingData") + "\n\n" + SqlError.text(), QueryText);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Create()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Create").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->Name, 0, parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, TableView, static_cast<void(ISBaseTableView::*)(void)>(&ISBaseTableView::setFocus));
	emit AddFormFromTab(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateCopy()
{
	if (CheckIsSystemObject())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.Copy"));
		return;
	}

	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.CreateCopy").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::OFT_Copy, MetaTable->Name, GetObjectID(), parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, TableView, static_cast<void(ISBaseTableView::*)(void)>(&ISBaseTableView::setFocus));
	emit AddFormFromTab(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Edit()
{
	if (CheckIsSystemObject())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.NotEdit"));
		return;
	}

	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, MetaTable->Name, GetObjectID(), parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, TableView, static_cast<void(ISBaseTableView::*)(void)>(&ISBaseTableView::setFocus));
	emit AddFormFromTab(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Update()
{
	SelectObjectAfterUpdate = GetSelectedIDs(); //���������� ���������� ������ ����� ����������� �������
	IsLoadingData = true; //������������� ���� ��� ������ ���������� �������� ������
	SqlModel->Clear();	 //������ ������

	QueryModel->SetParentFilter(GetParentObjectID(), GetParentFilterField());
	ModelThreadQuery->Execute(QueryModel->GetQueryText(), QueryModel->GetConditions());

	LabelSelectedRow->setVisible(false);
	LabelSelectedRow->clear();

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_PAGE_NAVIGATION))
	{
		PageNavigation->SetRowCount(ISDatabaseHelper::GetCountRows(MetaTable->Name));
	}
}
//-----------------------------------------------------------------------------
bool ISListBaseForm::Delete()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_DELETE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Delete").arg(MetaTable->LocalListName));
		return false;
	}

	QString ErrorString;
	bool Result = true;
	ISVectorInt VectorInt = GetSelectedIDs();

	if (CONFIG_BOOL("Protocol/Include"))
	{
		Result = VectorInt.size() == 1 ?
			ISMessageBox::ShowQuestion(this, LANG("Message.Object.Delete")) :
			ISMessageBox::ShowQuestion(this, LANG("Message.Objects.Delete").arg(VectorInt.size()));
		if (Result) //�������� ������������� �� ������������
		{
			Result = ISGui::RecordsDelete(MetaTable->Name, VectorInt, ErrorString);
			if (Result)
			{
				ISPopupMessage::ShowNotification(VectorInt.size() == 1 ?
					LANG("NotificationForm.Title.Deleted").arg(VectorInt.front()) :
					LANG("NotificationForm.Title.Deleteds"));
				Update();
			}
			else
			{
				ISMessageBox::ShowCritical(this, ErrorString);
			}
		}
	}
	else
	{
		if (VectorInt.size() == 1) //���� ��������� ���� ������
		{
			if (CheckIsSystemObject()) //���� ������ �������� ��������� - ������� �� �������
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.NotDelete"));
				return false;
			}

			if (ISMessageBox::ShowQuestion(this, LANG("Message.Object.Delete")))
			{
				int ObjectID = VectorInt.front();
				Result = ISCore::DeleteObject(MetaTable, ObjectID, ErrorString);
				if (Result)
				{
					ISPopupMessage::ShowNotification(LANG("NotificationForm.Title.Deleted").arg(ObjectID));
					ISProtocol::DeleteObject(MetaTable->Name, MetaTable->LocalListName, GetObjectID());
					Update();
				}
				else
				{
					ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteObject"), ErrorString);
				}
				return Result;
			}
		}
		else //��������� ��������� �������
		{
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Objects.Delete").arg(VectorInt.size())))
			{
				ISProgressForm ProgressForm((int)VectorInt.size(), LANG("DeletingObjects"), this);
				ProgressForm.show();

				for (int ObjectID : VectorInt)
				{
					ProgressForm.IncrementValue();
					Result = ISCore::DeleteObject(MetaTable, ObjectID, ErrorString);
					if (Result) //���� ������ ������� - �������������
					{
						ISProtocol::DeleteObject(MetaTable->Name, MetaTable->LocalListName, GetObjectID());
					}
					else //�� ������� ������� ������ - ������� �� �����
					{
						ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotDeleteRecord").arg(ObjectID), ErrorString);
						break;
					}

					if (ProgressForm.wasCanceled())
					{
						break;
					}
				}
				Update();
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Search()
{
	if (!SearchForm)
	{
		SearchForm = new ISSearchForm(MetaTable);
		connect(SearchForm, &ISSearchForm::StartSearch, [=](const QString &SearchString, const QVariantMap &VariantMap)
		{
			QueryModel->SetSearchFilter(SearchString);
			QueryModel->SetCondition(VariantMap);
			SearchFlag = true;
			Update();

			GetAction(ISNamespace::AT_SearchClear)->setEnabled(true);
			ISProtocol::Insert(true, CONST_UID_PROTOCOL_SEARCH, MetaTable->Name, MetaTable->LocalListName, QVariant());
		});
	}
	SearchForm->show();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SearchClear()
{
	GetQueryModel()->ClearConditions();
	GetQueryModel()->ClearSearchFilter();
	Update();
	GetAction(ISNamespace::AT_SearchClear)->setEnabled(false);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Export()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_EXPORT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Export").arg(MetaTable->LocalListName));
		return;
	}

	if (!SqlModel->rowCount())
	{
		ISMessageBox::ShowWarning(this, LANG("Export.NoData"));
		return;
	}

	ISExportForm ExportForm(MetaTable);
	if (!ExportForm.Exec())
	{
		return;
	}

	ISExportWorker *ExportWorker = nullptr;
	switch (ExportForm.GetSelectedType())
	{
	case ISNamespace::ET_CSV: ExportWorker = new ISExportCSV(MetaTable, this); break;
	case ISNamespace::ET_HTML: ExportWorker = new ISExportHTML(MetaTable, this); break;
	case ISNamespace::ET_DBF: ExportWorker = new ISExportDBF(MetaTable, this); break;
	case ISNamespace::ET_XML: ExportWorker = new ISExportXML(MetaTable, this); break;
	}
	ExportWorker->SetFields(ExportForm.GetSelectedFields());
	ExportWorker->SetModel(SqlModel);
	ExportWorker->SetHeader(ExportForm.GetHeader());
	ExportWorker->SetSelectedRows(GetSelectedRowIndexes());

	ISProgressForm ProgressForm(SqlModel->rowCount(), LANG("Export.Process.Prepare"), this);
	connect(&ProgressForm, &ISProgressForm::canceled, ExportWorker, &ISExportWorker::Cancel);
	connect(ExportWorker, &ISExportWorker::ExportedRow, &ProgressForm, static_cast<void(ISProgressForm::*)(void)>(&ISProgressForm::IncrementValue));
	ProgressForm.show();

	bool Prepared = ExportWorker->Prepare();
	if (Prepared)
	{
		bool Exported = ExportWorker->Export();
		if (Exported)
		{
			ProgressForm.close();
			ISProtocol::Insert(true, CONST_UID_PROTOCOL_EXPORT_TABLE, MetaTable->Name, MetaTable->LocalListName, QVariant(), ExportForm.GetSelectTypeName());
			ISMessageBox::ShowInformation(this, LANG("Export.Completed").arg(MetaTable->LocalListName).arg(ExportForm.GetSelectTypeName()));
		}
		else
		{
			ISMessageBox::ShowWarning(this, LANG("Export.Error.Export"), ExportWorker->GetErrorString());
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Export.Error.Prepare"), ExportWorker->GetErrorString());
	}
	POINTER_DELETE(ExportWorker);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Print()
{
	ISGui::SetWaitGlobalCursor(true);
	ISPrintForm PrintListForm(MetaTable->Name);
	ISGui::SetWaitGlobalCursor(false);
	PrintListForm.Exec();

	ISPrintMetaReport *MetaReport = PrintListForm.GetMetaReport();
	if (!MetaReport)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotGettingMetaReport"));
		return;
	}

	ISProtocol::Insert(true, CONST_UID_PROTOCOL_PRINT, MetaTable->Name, MetaTable->LocalListName, GetObjectID(), MetaReport->LocalName);
	ISGui::SetWaitGlobalCursor(true);

	ISProcessForm ProcessForm;
	ProcessForm.show();
	ProcessForm.SetText(LANG("PrintProcess.Preapre"));

	ISPrintingBase *PrintingBase = nullptr;
	bool EditPrintForm = ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_REPORT_FORM_EDIT);

	if (MetaReport->Type == ISNamespace::RT_Html)
	{
		PrintingBase = new ISPrintingHtml(MetaReport, GetObjectID(), this);
		PrintingBase->setProperty("PDF", PrintListForm.GetPDF());
		PrintingBase->setProperty("PathPDF", QDir::homePath() + '/' + MetaReport->LocalName);
		PrintingBase->setProperty("EditPreview", EditPrintForm);
	}

	PrintingBase->SetReportLocalName(MetaReport->LocalName);
	connect(PrintingBase, &ISPrintingBase::SetVisibleDialog, &ProcessForm, &ISProcessForm::setVisible);
	connect(PrintingBase, &ISPrintingBase::Message, &ProcessForm, &ISProcessForm::SetText);

	bool Result = false;

	Result = PrintingBase->Prepare();
	if (Result)
	{
		ProcessForm.SetText(LANG("PrintProcess.ReadFileTemplate"));
		Result = PrintingBase->PrepareTempate();
		if (Result)
		{
			ProcessForm.SetText(LANG("PrintProcess.FillTemplateData"));

			Result = PrintingBase->FillTemplate(); //���������� ������� �������
			if (Result)
			{
				if (PrintListForm.GetPreview()) //���� ������ ��������������� ��������
				{
					Result = PrintingBase->PreviewDocument();
				}

				if (Result)
				{
					ProcessForm.SetText(LANG("PrintProcess.Printing"));
					Result = PrintingBase->Print();
				}
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.ErrorFillTemplateData"), PrintingBase->GetErrorString());
			}
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.ErrorOpenedFileTemplatePrinting"), PrintingBase->GetErrorString());
		}
	}
	else
	{

	}

	ISGui::SetWaitGlobalCursor(false);
	POINTER_DELETE(PrintingBase);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowFavorites()
{
	ISGui::ShowFavoritesForm(MetaTable->Name);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NavigationSelectBeginRecord()
{
	if (SqlModel->rowCount())
	{
		TableView->selectRow(0);
		TableView->verticalScrollBar()->setValue(TableView->verticalScrollBar()->minimum());
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NavigationSelectPreviousRecord()
{
	if (SqlModel->rowCount() || GetCurrentRowIndex())
	{
		TableView->selectRow(GetCurrentRowIndex() - 1);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NavigationSelectNextRecord()
{
	if (SqlModel->rowCount() || GetCurrentRowIndex() == SqlModel->rowCount() - 1)
	{
		TableView->selectRow(GetCurrentRowIndex() + 1);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NavigationSelectLastRecord()
{
	if (SqlModel->rowCount())
	{
		TableView->selectRow(SqlModel->rowCount() - 1);
		TableView->verticalScrollBar()->setValue(TableView->verticalScrollBar()->maximum());
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NoteObject()
{
	ISGui::ShowNoteObject(this, MetaTable->Name, GetObjectID());
}
//-----------------------------------------------------------------------------
void ISListBaseForm::AutoFitColumnWidth()
{
	ISGui::SetWaitGlobalCursor(true);
	TableView->resizeColumnsToContents();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ResetWidthColumn()
{
	ISGui::SetWaitGlobalCursor(true);
	for (int i = 0; i < SqlModel->columnCount(); ++i)
	{
		TableView->setColumnWidth(i, 100);
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowContextMenu(const QPoint &Point)
{
	if (!IsLoadingData)
	{
		ContextMenu->exec(TableView->viewport()->mapToGlobal(Point));
	}
}
//-----------------------------------------------------------------------------
