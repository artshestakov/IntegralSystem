#include "ISListBaseForm.h"
#include "ISDefinesGui.h"
#include "ISAssert.h"
#include "ISCore.h"
#include "ISSettings.h"
#include "ISLocalization.h"
#include "ISProtocol.h"
#include "ISPopupMessage.h"
#include "ISExportDialog.h"
#include "ISProgressForm.h"
#include "ISLogger.h"
#include "ISProcessForm.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISMessageBox.h"
#include "ISUserRoleEntity.h"
#include "ISSystem.h"
#include "ISDatabaseHelper.h"
#include "ISDelegates.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISColumnSizer.h"
#include "ISInputDialog.h"
#include "ISExportWorker.h"
#include "ISQueryPool.h"
#include "ISGui.h"
#include "ISAlgorithm.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISListBaseForm::ISListBaseForm(const QString &TableName, QWidget *parent)
	: ISInterfaceMetaForm(parent),
	MetaTable(ISMetaData::Instance().GetMetaTable(TableName)),
	TcpQuery(new ISTcpQueryTable()),
	IsLoadingData(false),
	SelectObjectAfterUpdate(0),
	ActionObjectGroup(new QActionGroup(this)), //Группа действий, остосящихся только к одному объекту
	PageNavigation(nullptr),
	SearchForm(nullptr)
{
	{//Создание действий
		//Создать
		QAction *ActionCreate = ISControls::CreateActionCreate(this);
		ActionCreate->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		connect(ActionCreate, &QAction::triggered, this, &ISListBaseForm::Create);
		Actions[ISNamespace::AT_Create] = ActionCreate;

		//Создать копию
		QAction *ActionCreateCopy = ISControls::CreateActionCreateCopy(this);
		connect(ActionCreateCopy, &QAction::triggered, this, &ISListBaseForm::CreateCopy);
		Actions[ISNamespace::AT_CreateCopy] = ActionCreateCopy;
		ActionObjectGroup->addAction(ActionCreateCopy);
		
		//Изменить
		QAction *ActionEdit = ISControls::CreateActionEdit(this);
		connect(ActionEdit, &QAction::triggered, this, &ISListBaseForm::Edit);
		Actions[ISNamespace::AT_Edit] = ActionEdit;
		ActionObjectGroup->addAction(ActionEdit);

		//Удалить
		QAction *ActionDelete = ISControls::CreateActionDelete(this);
		connect(ActionDelete, &QAction::triggered, this, &ISListBaseForm::Delete);
		Actions[ISNamespace::AT_Delete] = ActionDelete;
		ActionObjectGroup->addAction(ActionDelete);

		//Обновить
		QAction *ActionUpdate = ISControls::CreateActionUpdate(this);
		connect(ActionUpdate, &QAction::triggered, this, &ISListBaseForm::Update);
		Actions[ISNamespace::AT_Update] = ActionUpdate;

		//Поиск
		QAction *ActionSearch = ISControls::CreateActionSearch(this);
		connect(ActionSearch, &QAction::triggered, this, &ISListBaseForm::Search);
		Actions[ISNamespace::AT_Search] = ActionSearch;

		//Очистка результатов поиска
		QAction *ActionSearchClearResult = ISControls::CreateActionSearchClearResults(this);
		ActionSearchClearResult->setEnabled(false);
		connect(ActionSearchClearResult, &QAction::triggered, this, &ISListBaseForm::SearchClear);
		Actions[ISNamespace::AT_SearchClear] = ActionSearchClearResult;

		//Экспорт
		QAction *ActionExport = new QAction(BUFFER_ICONS("ExportTable"), LANG("ListForm.ExportTable"), this);
		ActionExport->setShortcut(QKeySequence(Qt::Key_F12));
		connect(ActionExport, &QAction::triggered, this, &ISListBaseForm::Export);
		Actions[ISNamespace::AT_Export] = ActionExport;

		//Избранное
		QAction *ActionFavorites = new QAction(BUFFER_ICONS("Favorites"), LANG("Favorites"), this);
		connect(ActionFavorites, &QAction::triggered, this, &ISListBaseForm::ShowFavorites);
		Actions[ISNamespace::AT_Favorites] = ActionFavorites;

		//Первая запись
		QAction *ActionNavigationBegin = new QAction(BUFFER_ICONS("TableNavigationBegin"), LANG("TableNavigationSelectBegin"), this);
		ActionNavigationBegin->setShortcut(QKeySequence(Qt::Key_Home));
		connect(ActionNavigationBegin, &QAction::triggered, this, &ISListBaseForm::NavigationSelectBeginRecord);
		Actions[ISNamespace::AT_NavigationBegin] = ActionNavigationBegin;

		//Предыдущая запись
		QAction *ActionNavigationPrevious = new QAction(BUFFER_ICONS("TableNavigationPrevious"), LANG("TableNavigationSelectPrevious"), this);
		connect(ActionNavigationPrevious, &QAction::triggered, this, &ISListBaseForm::NavigationSelectPreviousRecord);
		Actions[ISNamespace::AT_NavigationPrevious] = ActionNavigationPrevious;

		//Следующая запись
		QAction *ActionNavigationNext = new QAction(BUFFER_ICONS("TableNavigationNext"), LANG("TableNavigationSelectNext"), this);
		connect(ActionNavigationNext, &QAction::triggered, this, &ISListBaseForm::NavigationSelectNextRecord);
		Actions[ISNamespace::AT_NavigationNext] = ActionNavigationNext;

		//Последняя запись
		QAction *ActionNavigationLast = new QAction(BUFFER_ICONS("TableNavigationLast"), LANG("TableNavigationSelectLast"), this);
		ActionNavigationLast->setShortcut(QKeySequence(Qt::Key_End));
		connect(ActionNavigationLast, &QAction::triggered, this, &ISListBaseForm::NavigationSelectLastRecord);
		Actions[ISNamespace::AT_NavigationLast] = ActionNavigationLast;
	}

	{//Создание специальных действий
		//Примечание
		QAction *ActionNoteObject = ISControls::CreateActionNoteObject(this);
		connect(ActionNoteObject, &QAction::triggered, this, &ISListBaseForm::NoteObject);
		ActionsSpecial[ISNamespace::AST_Note] = ActionNoteObject;
		ActionObjectGroup->addAction(ActionNoteObject);
	}

	{//Создание тулбара
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

		QAction *ActionAdditionally = ToolBar->addAction(BUFFER_ICONS("AdditionallyActions"), LANG("Additionally"));
		dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionAdditionally))->setPopupMode(QToolButton::InstantPopup);
		dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionAdditionally))->setStyleSheet(BUFFER_STYLE_SHEET("QToolButtonMenu"));
		ActionAdditionally->setMenu(new QMenu(ToolBar));
		ActionAdditionally->menu()->addAction(GetAction(ISNamespace::AT_Favorites));
		ActionAdditionally->menu()->addAction(GetAction(ISNamespace::AT_Export));
		ActionAdditionally->menu()->addAction(LANG("SettingsList"), this, &ISListBaseForm::ShowSettingsForm);

		if (SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWNAVIGATION))
		{
			ToolBar->addAction(GetAction(ISNamespace::AT_NavigationBegin));
			ToolBar->addAction(GetAction(ISNamespace::AT_NavigationPrevious));
			ToolBar->addAction(GetAction(ISNamespace::AT_NavigationNext));
			ToolBar->addAction(GetAction(ISNamespace::AT_NavigationLast));
		}
	}

	{//Создание таблицы
		TableView = new ISBaseTableView(this);
		TableView->SetCornerText(LANG("Reduction.SerialNumber"));
		TableView->SetCornerToolTip(LANG("OrdinalNumber"));
		connect(TableView, &ISBaseTableView::doubleClicked, this, &ISListBaseForm::DoubleClickedTable);
		connect(TableView, &ISBaseTableView::customContextMenuRequested, this, &ISListBaseForm::ShowContextMenu);
		connect(TableView, &ISBaseTableView::CornerClicked, this, &ISListBaseForm::CornerButtonClicked);
		FieldResized(true);
		GetMainLayout()->addWidget(TableView);

		if (SETTING_BOOL(CONST_UID_SETTING_TABLES_MINIMIZEHEIGHTROWS))
		{
			TableView->verticalHeader()->setDefaultSectionSize(19);
		}
		TableView->horizontalHeader()->setVisible(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWHORIZONTALHEADER));
		TableView->SetVisibleVerticalHeader(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWVERTICALHEADER));
	}

	{//Создание моделей
		TcpModel = new ISTcpModel(TableView);
		TableView->setModel(TcpModel);

		//Это соединение обязательно должно быть после присваивания модели к QTableView
		connect(TableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ISListBaseForm::SelectedRowEvent);
	}
	
	{//Создание статус-бара
		StatusBar = new QStatusBar(this);
		StatusBar->setSizeGripEnabled(false);
		GetMainLayout()->addWidget(StatusBar);

		LabelRowCount = new QLabel(LANG("RecordsCount") + ": 0", StatusBar);
		LabelRowCount->setVisible(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWCOUNTRECORD));
		StatusBar->addWidget(LabelRowCount);

		if (SETTING_BOOL(CONST_UID_SETTING_TABLES_PAGE_NAVIGATION))
		{
			//QueryModel->SetLimit(SETTING_INT(CONST_UID_SETTING_TABLES_PAGE_NAVIGATION_LIMIT));

			PageNavigation = new ISPageNavigation(StatusBar);
			PageNavigation->SetLimit(SETTING_INT(CONST_UID_SETTING_TABLES_PAGE_NAVIGATION_LIMIT));
			//connect(PageNavigation, &ISPageNavigation::OffsetSignal, QueryModel, &ISQueryModel::SetOffset);
			connect(PageNavigation, &ISPageNavigation::Update, this, &ISListBaseForm::Update);
			StatusBar->addWidget(PageNavigation);
		}

		LabelSelectedRow = new QLabel(StatusBar);
		LabelSelectedRow->setVisible(false);
		StatusBar->addWidget(LabelSelectedRow);
	}

	{//Создание контекстного меню
		ContextMenu = new QMenu(this);
		if (GetAction(ISNamespace::AT_Create)) ContextMenu->addAction(GetAction(ISNamespace::AT_Create));
		if (GetAction(ISNamespace::AT_CreateCopy)) ContextMenu->addAction(GetAction(ISNamespace::AT_CreateCopy));
		if (GetAction(ISNamespace::AT_Edit)) ContextMenu->addAction(GetAction(ISNamespace::AT_Edit));
		if (GetAction(ISNamespace::AT_Delete)) ContextMenu->addAction(GetAction(ISNamespace::AT_Delete));
		if (GetAction(ISNamespace::AT_Update)) ContextMenu->addAction(GetAction(ISNamespace::AT_Update));
		ContextMenu->addAction(GetSpecialAction(ISNamespace::AST_Note));
	}

	//Создание этого виджета должно происходить после создания всех остальных
	ListIndicatorWidget = new ISListIndicatorWidget(this);
}
//-----------------------------------------------------------------------------
ISListBaseForm::~ISListBaseForm()
{
	delete TcpQuery;
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
unsigned int ISListBaseForm::GetObjectID()
{
	return GetCurrentRecord().ID;
}
//-----------------------------------------------------------------------------
unsigned int ISListBaseForm::GetObjectID(int RowIndex)
{
	return TcpModel->index(RowIndex, 0).data().toInt();
}
//-----------------------------------------------------------------------------
int ISListBaseForm::GetRowIndex(int object_id)
{
	for (int i = 0; i < TcpModel->rowCount(); ++i)
	{
		int RowIndex = TcpModel->data(TcpModel->index(i, 0)).toInt();
		if (RowIndex == object_id)
		{
			return i;
		}
	}
	return -1;
}
//-----------------------------------------------------------------------------
const ISModelRecord& ISListBaseForm::GetCurrentRecord()
{
	return TcpModel->GetRecord(GetCurrentRowIndex());
}
//-----------------------------------------------------------------------------
QVariant ISListBaseForm::GetCurrentRecordValue(const QString &FieldName)
{
	return TcpModel->GetRecordValue(GetCurrentRowIndex(), FieldName);
}
//-----------------------------------------------------------------------------
QVariant ISListBaseForm::GetCurrentRecordValueDB(const QString &FieldName)
{
	QVariant Value;
	ISTcpQuery qGetRecordValue(API_GET_RECORD_VALUE);
	qGetRecordValue.BindValue("TableName", MetaTable->Name);
	qGetRecordValue.BindValue("FieldName", FieldName);
	qGetRecordValue.BindValue("ObjectID", GetObjectID());
	if (qGetRecordValue.Execute())
	{
		Value = qGetRecordValue.GetAnswer()["Value"];
	}
	else
	{
		ISMessageBox::ShowCritical(this, qGetRecordValue.GetErrorString());
	}
	return Value;
}
//-----------------------------------------------------------------------------
ISVectorUInt ISListBaseForm::GetSelectedIDs()
{
	ISVectorUInt VectorInt;
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
ISVectorUInt ISListBaseForm::GetIDs() const
{
	return TcpModel->GetIDs();
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
void ISListBaseForm::SetSelectObjectAfterUpdate(unsigned int ObjectID)
{
	SetSelectObjectAfterUpdate(ISVectorUInt{ ObjectID });
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SetSelectObjectAfterUpdate(const ISVectorUInt &Objects)
{
	SelectObjectAfterUpdate = Objects;
}
//-----------------------------------------------------------------------------
PMetaTable* ISListBaseForm::GetMetaTable()
{
	return MetaTable;
}
//-----------------------------------------------------------------------------
ISTcpModel* ISListBaseForm::GetTcpModel()
{
	return TcpModel;
}
//-----------------------------------------------------------------------------
QStatusBar* ISListBaseForm::GetStatusBar()
{
	return StatusBar;
}
//-----------------------------------------------------------------------------
ISTcpQueryTable* ISListBaseForm::GetTcpQuery()
{
	return TcpQuery;
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
		connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(unsigned int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
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
	Include ?
		connect(TableView->horizontalHeader(), &QHeaderView::sectionResized, this, static_cast<void(ISListBaseForm::*)(int, int, int)>(&ISListBaseForm::FieldResized)) :
		disconnect(TableView->horizontalHeader(), &QHeaderView::sectionResized, this, static_cast<void(ISListBaseForm::*)(int, int, int)>(&ISListBaseForm::FieldResized));
}
//-----------------------------------------------------------------------------
void ISListBaseForm::FieldResized(int LogicalIndex, int WidthOld, int WidthNew)
{
	Q_UNUSED(WidthOld);
	ISColumnSizer::Instance().SetColumnSize(MetaTable->Name, TcpModel->headerData(LogicalIndex, Qt::Horizontal, Qt::UserRole).toString(), WidthNew);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SortingChanged(int LogicalIndex, Qt::SortOrder SortingOrder)
{
	TcpQuery->SetSorting(TcpModel->headerData(LogicalIndex, Qt::Horizontal, Qt::UserRole).toString(), SortingOrder);
	Update();
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
	if (Update())
	{
		CreateDelegates();
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CornerButtonClicked()
{
	if (TcpModel->rowCount())
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
	for (int i = 0; i < TcpModel->columnCount(); ++i) //Обход полей
	{
		QString HeaderData = TcpModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString();
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
	TableView->hideColumn(TcpModel->GetFieldIndex(FieldName));
	FieldResized(true);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowField(const QString &FieldName)
{
	FieldResized(false);
	TableView->showColumn(TcpModel->GetFieldIndex(FieldName));
	FieldResized(true);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SelectRowObject(const ISVectorUInt &Objects)
{
	if (!Objects.empty()) //Если вектор не пустой
	{
		//Устанавливаем режим выделения на мультистрочный (потому что если этого не сделать,
		//то сколько бы строк в Objects не было, выделена будет только одна строка)
		TableView->setSelectionMode(QAbstractItemView::MultiSelection);

		//Обходим все записи
		for (int ObjectID : Objects)
		{
			int RowIndex = GetRowIndex(ObjectID); //Получаем индекс строки по её идентификатору
			if (RowIndex != -1) //Если индекс валидный - выделяем строку
			{
				SelectRowIndex(RowIndex);
			}
		}

		//Возвращаем режим выделения на исходный и очищаем вектор выделения
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
void ISListBaseForm::Create()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Create").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->Name, 0, parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(unsigned int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
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
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(unsigned int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
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
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(unsigned int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, TableView, static_cast<void(ISBaseTableView::*)(void)>(&ISBaseTableView::setFocus));
	emit AddFormFromTab(ObjectFormBase);
}
//-----------------------------------------------------------------------------
bool ISListBaseForm::Update()
{
	SetSelectObjectAfterUpdate(GetSelectedIDs()); //Запоминаем выделенные записи перед обновлением таблицы
	IsLoadingData = true; //Устанавливаем флаг что сейчас происходит загрузка данных
	TcpModel->Clear(); //Чистим модель

	//Проводим операции с интерфейсом перед загрузкой
	ISGui::SetWaitGlobalCursor(true);
	ListIndicatorWidget->SetIcon(QIcon());
	ListIndicatorWidget->SetVisibleAnimation(true);
	ListIndicatorWidget->SetText(LANG("LoadDataPleceWait"));
	ListIndicatorWidget->show();
	repaint(); //Нужно для корректной отрисовки виджета ListIndicatorWidget
	ToolBar->setEnabled(false);
	SetEnabledPageNavigation(false);

	//Готовим запрос и исполняем
	TcpQuery->BindValue("TableName", MetaTable->Name);
	bool Result = TcpQuery->Execute();

	//Очередные операции с интерфейсом после загрузки
	ToolBar->setEnabled(true);
	SetEnabledPageNavigation(true);
	ISGui::SetWaitGlobalCursor(false);

	if (Result) //Запрос прошёл успешно
	{
		//Забираем ответ и устанавливаем его в модель
		ListIndicatorWidget->SetText(LANG("FillTableData"));
		QVariantMap AnswerMap = TcpQuery->TakeAnswer();
		TcpModel->SetSource(AnswerMap["FieldList"].toList(), AnswerMap["RecordList"].toList());

		ListIndicatorWidget->SetVisibleAnimation(false); //Выключаем анимацию

		QVariantMap ServiceInfo = AnswerMap["ServiceInfo"].toMap();
		QString SortingField = ServiceInfo["SortingField"].toString();
		Qt::SortOrder SortingOrder = static_cast<Qt::SortOrder>(ServiceInfo["SortingOrder"].toUInt());
		TcpModel->SetSorting(SortingField, SortingOrder);

		//Устанавливаем индикатор сортировки
		disconnect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISListBaseForm::SortingChanged);
		TableView->horizontalHeader()->setSortIndicator(TcpModel->GetFieldIndex(SortingField), SortingOrder);
		connect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISListBaseForm::SortingChanged);

		//Устанавливаем размеры полей
		FieldResized(false);
		for (int i = 0, c = TcpModel->columnCount(); i < c; ++i)
		{
			int ColumnSize = ISColumnSizer::Instance().GetColumnSize(MetaTable->Name,
				TcpModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString());
			if (ColumnSize)
			{
				TableView->setColumnWidth(i, ColumnSize);
			}
		}
		FieldResized(true);

		LabelRowCount->setText(QString("%1: %2").arg(LANG("RecordsCount")).arg(TcpModel->rowCount()));
		
		if (!SETTING_BOOL(CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID))
		{
			HideField("ID");
		}
		HideField("IsSystem");

		SetEnabledActionObject(false);
		SelectRowObject(SelectObjectAfterUpdate);
		
		if (!TcpModel->rowCount()) //Записей в таблице нет
		{
			ListIndicatorWidget->SetIcon(BUFFER_ICONS("LabelNoDataTable"));
			ListIndicatorWidget->SetText(LANG("NoData"));
		}
		else //Записи есть - скрываем индикатор
		{
			ListIndicatorWidget->hide();
		}

		LoadDataAfterEvent();
		emit Updated();
	}
	else //Не удалось получить данные - выключаем анимацию и выдаём окно с ошибкой
	{
		ListIndicatorWidget->SetVisibleAnimation(false);
		ListIndicatorWidget->SetText(LANG("ErrorLoadingData"));
		ListIndicatorWidget->SetIcon(BUFFER_ICONS("ListIndicator.Error"));
		ISMessageBox::ShowCritical(this, TcpQuery->GetErrorString());
	}

	//if (SETTING_BOOL(CONST_UID_SETTING_TABLES_PAGE_NAVIGATION))
	{
		//PageNavigation->SetRowCount(ISDatabaseHelper::GetCountRows(MetaTable->Name));
	}

	IsLoadingData = false;
	return Result;
}
//-----------------------------------------------------------------------------
bool ISListBaseForm::Delete()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_DELETE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Delete").arg(MetaTable->LocalListName));
		return false;
	}

	ISVectorUInt VectorInt = GetSelectedIDs();
	bool Result = VectorInt.size() == 1 ?
		ISMessageBox::ShowQuestion(this, LANG("Message.Object.Delete")) :
		ISMessageBox::ShowQuestion(this, LANG("Message.Objects.Delete").arg(VectorInt.size()));
	if (Result) //Получили подтверждение от пользователя
	{
		QString ErrorString;
		Result = ISGui::RecordsDelete(MetaTable->Name, VectorInt, ErrorString);
		if (Result)
		{
			for (unsigned int ObjectID : VectorInt)
			{
				TcpModel->RemoveRecordID(ObjectID);
			}
		}
		else
		{
			ISMessageBox::ShowCritical(this, ErrorString);
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
			//???
			Q_UNUSED(SearchString);
			Q_UNUSED(VariantMap);
			//QueryModel->SetSearchFilter(SearchString);
			//QueryModel->SetCondition(VariantMap);
			Update();

			GetAction(ISNamespace::AT_SearchClear)->setEnabled(true);
			ISProtocol::Insert(CONST_UID_PROTOCOL_SEARCH, MetaTable->Name, MetaTable->LocalListName, QVariant());
		});
	}
	SearchForm->show();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SearchClear()
{
	//GetQueryModel()->ClearConditions();
	//GetQueryModel()->ClearSearchFilter();
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

	if (!TcpModel->rowCount())
	{
		ISMessageBox::ShowWarning(this, LANG("Export.NoData"));
		return;
	}

	ISExportDialog ExportForm(TcpModel);
	if (!ExportForm.Exec())
	{
		return;
	}

	ISExportWorker *ExportWorker = nullptr;
	switch (ExportForm.GetSelectedType())
	{
	case ISNamespace::ET_CSV: ExportWorker = new ISExportCSV(MetaTable, TcpModel, this); break;
	case ISNamespace::ET_HTML: ExportWorker = new ISExportHTML(MetaTable, TcpModel, this); break;
	case ISNamespace::ET_DBF: ExportWorker = new ISExportDBF(MetaTable, TcpModel, this); break;
	case ISNamespace::ET_XML: ExportWorker = new ISExportXML(MetaTable, TcpModel, this); break;
	case ISNamespace::ET_JSON: ExportWorker = new ISExportJSON(MetaTable, TcpModel, this); break;
	case ISNamespace::ET_SQLITE: ExportWorker = new ISExportSQLite(MetaTable, TcpModel, this); break;
	}
	ExportWorker->SetFields(ExportForm.GetSelectedFields());
	ExportWorker->SetHeader(ExportForm.GetHeader());
	ExportWorker->SetSelectedRows(GetSelectedRowIndexes());

	ISProgressForm ProgressForm(TcpModel->rowCount(), LANG("Export.Process.Prepare"), this);
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
			ISProtocol::Insert(CONST_UID_PROTOCOL_EXPORT_TABLE, MetaTable->Name, MetaTable->LocalListName, QVariant(), ExportForm.GetSelectTypeName());
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
void ISListBaseForm::ShowFavorites()
{
	ISGui::ShowFavoritesForm(MetaTable->Name);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NavigationSelectBeginRecord()
{
	if (TcpModel->rowCount())
	{
		TableView->selectRow(0);
		TableView->verticalScrollBar()->setValue(TableView->verticalScrollBar()->minimum());
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NavigationSelectPreviousRecord()
{
	if (TcpModel->rowCount() || GetCurrentRowIndex())
	{
		TableView->selectRow(GetCurrentRowIndex() - 1);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NavigationSelectNextRecord()
{
	int RowCount = TcpModel->rowCount();
	if (RowCount || GetCurrentRowIndex() == RowCount - 1)
	{
		TableView->selectRow(GetCurrentRowIndex() + 1);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NavigationSelectLastRecord()
{
	int RowCount = TcpModel->rowCount();
	if (RowCount)
	{
		TableView->selectRow(RowCount - 1);
		TableView->verticalScrollBar()->setValue(TableView->verticalScrollBar()->maximum());
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::NoteObject()
{
	ISGui::ShowNoteObject(this, MetaTable->Name, GetObjectID());
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
