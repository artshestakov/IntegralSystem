#include "ISListBaseForm.h"
#include "ISDefinesGui.h"
#include "ISAssert.h"
#include "ISSettings.h"
#include "ISLocalization.h"
#include "ISPopupMessage.h"
#include "ISProgressForm.h"
#include "ISLogger.h"
#include "ISProcessForm.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISDialogsCommon.h"
#include "ISUserRoleEntity.h"
#include "ISDelegates.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISColumnSizer.h"
#include "ISExportWorker.h"
#include "ISGui.h"
#include "ISAlgorithm.h"
#include "ISTcpQuery.h"
#include "ISDialogsForm.h"
#include "ISFavorites.h"
//-----------------------------------------------------------------------------
ISListBaseForm::ISListBaseForm(const QString &TableName, QWidget *parent)
	: ISInterfaceMetaForm(parent),
	MetaTable(ISMetaData::Instance().GetMetaTable(TableName)),
	TcpQuery(new ISTcpQueryTable()),
	IsLoadingData(false),
	SelectObjectAfterUpdate(0),
	ActionObjectGroup(new QActionGroup(this)),
	ActionGroupPageNavigation(new QActionGroup(this)),
	SearchForm(nullptr)
{
	{//Создание действий
		//Создать
		QAction *ActionCreate = ISControls::CreateActionCreate(this);
		ActionCreate->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		connect(ActionCreate, &QAction::triggered, this, &ISListBaseForm::Create);
		Actions[ISNamespace::ActionType::Create] = ActionCreate;

		//Создать копию
		QAction *ActionCreateCopy = ISControls::CreateActionCreateCopy(this);
		connect(ActionCreateCopy, &QAction::triggered, this, &ISListBaseForm::CreateCopy);
		Actions[ISNamespace::ActionType::CreateCopy] = ActionCreateCopy;
		ActionObjectGroup->addAction(ActionCreateCopy);
		
		//Изменить
		QAction *ActionEdit = ISControls::CreateActionEdit(this);
		connect(ActionEdit, &QAction::triggered, this, &ISListBaseForm::Edit);
		Actions[ISNamespace::ActionType::Edit] = ActionEdit;
		ActionObjectGroup->addAction(ActionEdit);

		//Удалить
		QAction *ActionDelete = ISControls::CreateActionDelete(this);
		connect(ActionDelete, &QAction::triggered, this, &ISListBaseForm::Delete);
		Actions[ISNamespace::ActionType::Delete] = ActionDelete;
		ActionObjectGroup->addAction(ActionDelete);

		//Обновить
		QAction *ActionUpdate = ISControls::CreateActionUpdate(this);
		connect(ActionUpdate, &QAction::triggered, this, &ISListBaseForm::Update);
		Actions[ISNamespace::ActionType::Update] = ActionUpdate;

		//Поиск
		QAction *ActionSearch = ISControls::CreateActionSearch(this);
		connect(ActionSearch, &QAction::triggered, this, &ISListBaseForm::SearchShow);
		Actions[ISNamespace::ActionType::Search] = ActionSearch;

		//Очистка результатов поиска
		QAction *ActionSearchClearResult = ISControls::CreateActionSearchClearResults(this);
		connect(ActionSearchClearResult, &QAction::triggered, this, &ISListBaseForm::SearchClear);
		Actions[ISNamespace::ActionType::SearchClear] = ActionSearchClearResult;

		//Экспорт
		QAction *ActionExport = new QAction(BUFFER_ICONS("ExportTable"), LANG("ListForm.ExportTable"), this);
		ActionExport->setShortcut(QKeySequence(Qt::Key_F12));
		connect(ActionExport, &QAction::triggered, this, &ISListBaseForm::Export);
		Actions[ISNamespace::ActionType::Export] = ActionExport;

		//Избранное
		QAction *ActionFavorites = new QAction(BUFFER_ICONS("Favorites"), LANG("Favorites"), this);
		connect(ActionFavorites, &QAction::triggered, this, &ISListBaseForm::ShowFavorites);
		Actions[ISNamespace::ActionType::Favorites] = ActionFavorites;
	}

	{//Создание специальных действий
		//Информация о записи
		QAction *ActionRecordInfo = ISControls::CreateActionRecordInfo(this);
		connect(ActionRecordInfo, &QAction::triggered, this, &ISListBaseForm::RecordInfo);
		ActionsSpecial[ISNamespace::ActionSpecialType::RecordInfo] = ActionRecordInfo;
		ActionObjectGroup->addAction(ActionRecordInfo);

		//Избранное
		QAction *ActionFavorite = ISControls::CreateActionFavorite(this);
		ActionFavorite->setCheckable(true);
		connect(ActionFavorite, &QAction::triggered, this, &ISListBaseForm::FavoriteObject);
		ActionsSpecial[ISNamespace::ActionSpecialType::Favorite] = ActionFavorite;
		ActionObjectGroup->addAction(ActionFavorite);

		//Примечание
		QAction *ActionNoteObject = ISControls::CreateActionNoteObject(this);
		connect(ActionNoteObject, &QAction::triggered, this, &ISListBaseForm::NoteObject);
		ActionsSpecial[ISNamespace::ActionSpecialType::Note] = ActionNoteObject;
		ActionObjectGroup->addAction(ActionNoteObject);
	}

	{//Создание тулбара
		ToolBar = new QToolBar(this);
		ToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		GetMainLayout()->addWidget(ToolBar);

		ToolBar->addAction(GetAction(ISNamespace::ActionType::Create));
		ToolBar->addAction(GetAction(ISNamespace::ActionType::CreateCopy));
		ToolBar->addAction(GetAction(ISNamespace::ActionType::Edit));
		ToolBar->addAction(GetAction(ISNamespace::ActionType::Delete));
		ToolBar->addAction(GetAction(ISNamespace::ActionType::Update));
		ToolBar->addAction(GetAction(ISNamespace::ActionType::Search));
		ToolBar->addAction(GetAction(ISNamespace::ActionType::SearchClear));

		QAction *ActionAdditionally = ToolBar->addAction(BUFFER_ICONS("AdditionallyActions"), LANG("Additionally"));
		dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionAdditionally))->setPopupMode(QToolButton::InstantPopup);
		dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionAdditionally))->setStyleSheet(BUFFER_STYLE_SHEET("QToolButtonMenu"));
		ActionAdditionally->setMenu(new QMenu(ToolBar));
		ActionAdditionally->menu()->addAction(GetAction(ISNamespace::ActionType::Favorites));
		ActionAdditionally->menu()->addAction(GetAction(ISNamespace::ActionType::Export));
		ActionAdditionally->menu()->addAction(LANG("SettingsList"), this, &ISListBaseForm::ShowSettingsForm);
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

		LabelSelectedRow = new QLabel(StatusBar);
		LabelSelectedRow->setVisible(false);
		StatusBar->addWidget(LabelSelectedRow);
	}

	{//Создание контекстного меню
		ContextMenu = new QMenu(this);
		if (GetAction(ISNamespace::ActionType::Create)) ContextMenu->addAction(GetAction(ISNamespace::ActionType::Create));
		if (GetAction(ISNamespace::ActionType::CreateCopy)) ContextMenu->addAction(GetAction(ISNamespace::ActionType::CreateCopy));
		if (GetAction(ISNamespace::ActionType::Edit)) ContextMenu->addAction(GetAction(ISNamespace::ActionType::Edit));
		if (GetAction(ISNamespace::ActionType::Delete)) ContextMenu->addAction(GetAction(ISNamespace::ActionType::Delete));
		if (GetAction(ISNamespace::ActionType::Update)) ContextMenu->addAction(GetAction(ISNamespace::ActionType::Update));
		ContextMenu->addAction(GetSpecialAction(ISNamespace::ActionSpecialType::RecordInfo));
		ContextMenu->addAction(GetSpecialAction(ISNamespace::ActionSpecialType::Favorite));
		ContextMenu->addAction(GetSpecialAction(ISNamespace::ActionSpecialType::Note));
	}
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
ISObjectPair ISListBaseForm::GetObject()
{
	ISModelRecord CurrentRecord = GetCurrentRecord();
	QString ObjectName;
	QStringList TitleNameFields = MetaTable->TitleName.split(';');
	for (const QString &FieldName : TitleNameFields)
	{
		ObjectName += TcpModel->data(TcpModel->index(GetCurrentRowIndex(), TcpModel->GetFieldIndex(FieldName))).toString() + " ";
	}
	ObjectName.chop(1);
	return ISObjectPair(CurrentRecord.ID, ObjectName);
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
	Edit();
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

	GetAction(ISNamespace::ActionType::Delete)->setEnabled(SelectedRows);
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
void ISListBaseForm::AfterShowEvent()
{
	ISInterfaceMetaForm::AfterShowEvent();
	SetShowOnly(MetaTable->ShowOnly);
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
void ISListBaseForm::Search(const QVariantList &VariantList)
{
	TcpQuery->SetSearch(VariantList);
	Update();
	GetAction(ISNamespace::ActionType::SearchClear)->setEnabled(true);
	
	if (!TcpModel->rowCount()) //Поиск не дал результатов
	{
		ListIndicatorWidget->SetIcon(BUFFER_ICONS("Search.EmptyResult"));
		ListIndicatorWidget->SetText(LANG("Search.EmptyResult"));
	}
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
			case ISNamespace::FieldType::Bool: AbstractItemDelegate = new ISDelegateBoolean(TableView); break;
			case ISNamespace::FieldType::Image: AbstractItemDelegate = new ISDelegateImage(TableView); break;
			case ISNamespace::FieldType::Color: AbstractItemDelegate = new ISDelegateColor(TableView); break;
			case ISNamespace::FieldType::File: AbstractItemDelegate = new ISDelegateFile(TableView); break;
			case ISNamespace::FieldType::TaskImportant: AbstractItemDelegate = new ISDelegateTaskImportant(TableView); break;
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
void ISListBaseForm::SetShowOnly(bool ShowOnly)
{
	GetAction(ISNamespace::ActionType::Create)->setVisible(!ShowOnly);
	GetAction(ISNamespace::ActionType::CreateCopy)->setVisible(!ShowOnly);
	GetAction(ISNamespace::ActionType::Edit)->setVisible(!ShowOnly);
	GetAction(ISNamespace::ActionType::Delete)->setVisible(!ShowOnly);
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
void ISListBaseForm::SetEnabledActions(bool Enabled)
{
	QList<QAction*> List = ToolBar->actions() + ContextMenu->actions();
	for (QAction *Action : List)
	{
		Action->setEnabled(Enabled);
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

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::New, MetaTable->Name, 0, parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(unsigned int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::DeletedObject, TcpModel, &ISTcpModel::RemoveRecordID);
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

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Copy, MetaTable->Name, GetObjectID(), parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(unsigned int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::DeletedObject, TcpModel, &ISTcpModel::RemoveRecordID);
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
	
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, MetaTable->Name, GetObjectID(), parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(unsigned int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::DeletedObject, TcpModel, &ISTcpModel::RemoveRecordID);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, TableView, static_cast<void(ISBaseTableView::*)(void)>(&ISBaseTableView::setFocus));

	//Учитываем настройку ""
	QString EventName = SETTING_STRING(CONST_UID_SETTING_TABLES_DOUBLECLICKEVENT);
	if (EventName == "Window")
	{
		ISGui::ShowObjectForm(ObjectFormBase);
	}
	else if (EventName == "Tab")
	{
		emit AddFormFromTab(ObjectFormBase);
	}
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
	SetEnabledActions(false);

	//Готовим запрос и исполняем
	TcpQuery->BindValue("TableName", MetaTable->Name);
	bool Result = TcpQuery->Execute();

	//Очередные операции с интерфейсом после загрузки
	SetEnabledActions(Result);
	GetAction(ISNamespace::ActionType::Update)->setEnabled(true); //Включаем только для обновления списка
	GetAction(ISNamespace::ActionType::SearchClear)->setEnabled(false);
	ISGui::SetWaitGlobalCursor(false);

	if (Result) //Запрос прошёл успешно
	{
		//Устанавливаем надпись "Заполнение таблицы"
		ListIndicatorWidget->SetText(LANG("FillTableData"));
		ISSleep(1);
		PROCESS_EVENTS();

		//Забираем ответ и устанавливаем его в модель
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
		emit Updated();
	}
	else //Не удалось получить данные - выключаем анимацию и выдаём окно с ошибкой
	{
		ListIndicatorWidget->SetVisibleAnimation(false);
		ListIndicatorWidget->SetText(LANG("ErrorLoadingData"));
		ListIndicatorWidget->SetIcon(BUFFER_ICONS("ListIndicator.Error"));
		ISMessageBox::ShowCritical(this, TcpQuery->GetErrorString());
	}

	ActionGroupPageNavigation->setEnabled(TcpModel->rowCount() > 0);
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
void ISListBaseForm::SearchShow()
{
	if (!SearchForm)
	{
		SearchForm = new ISSearchForm(MetaTable);
		connect(SearchForm, static_cast<void(ISSearchForm::*)(const QVariantList &)>(&ISSearchForm::Search), this, &ISListBaseForm::Search);
	}
	SearchForm->show();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SearchClear()
{
	GetAction(ISNamespace::ActionType::SearchClear)->setEnabled(false);
	TcpQuery->SetSearch(QVariantList());
	Update();
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
	case ISNamespace::ExportType::CSV: ExportWorker = new ISExportCSV(MetaTable, TcpModel, this); break;
	case ISNamespace::ExportType::HTML: ExportWorker = new ISExportHTML(MetaTable, TcpModel, this); break;
	case ISNamespace::ExportType::XML: ExportWorker = new ISExportXML(MetaTable, TcpModel, this); break;
	case ISNamespace::ExportType::JSON: ExportWorker = new ISExportJSON(MetaTable, TcpModel, this); break;
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
			//ISProtocol::Insert(CONST_UID_PROTOCOL_EXPORT_TABLE, MetaTable->Name, MetaTable->LocalListName, QVariant(), ExportForm.GetSelectTypeName());
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
void ISListBaseForm::RecordInfo()
{
	ISGui::ShowRecordInfoForm(this, MetaTable->Name, GetObjectID());
}
//-----------------------------------------------------------------------------
void ISListBaseForm::FavoriteObject()
{
	bool IsExist = true;
	ISGui::FavoriteObject(MetaTable->Name, GetObjectID(), IsExist);
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
		GetSpecialAction(ISNamespace::ActionSpecialType::Favorite)->setChecked(ISFavorites::Instance().Exist(MetaTable->Name, GetObjectID()));
		ContextMenu->exec(TableView->viewport()->mapToGlobal(Point));
	}
}
//-----------------------------------------------------------------------------
