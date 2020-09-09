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
#include "ISStyleSheet.h"
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
#include "ISPeriodForm.h"
#include "ISQueryPool.h"
#include "ISGui.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QI_SEARCH_FAST = PREPARE_QUERY("INSERT INTO _searchfast(srfs_value) "
											  "VALUES(:Value)");
//-----------------------------------------------------------------------------
ISListBaseForm::ISListBaseForm(const QString &TableName, QWidget *parent)
	: ISInterfaceMetaForm(parent),
	ActionObjectGroup(new QActionGroup(this)), //Группа действий, остосящихся только к одному объекту
	MetaTable(ISMetaData::Instance().GetMetaTable(TableName)),
	SqlModel(nullptr),
	PageNavigation(nullptr),
	QueryModel(nullptr),
	SearchForm(nullptr),
	SelectObjectAfterUpdate(0),
	DelegatesCreated(false),
	ShowOnly(MetaTable->ShowOnly),
	IsLoadingData(false),
	SearchFlag(false)
{
	CreateActions(); //Создание действий
	CreateSpecialActions(); //Создание специальных действий
	CreateToolBar(); //Создание тулбара
	CreateTableView(); //Создание таблицы
	CreateContextMenu(); //Создание контекстного меню
	CreateModels(); //Создание моделей
	CreateStatusBar(); //Создание статус-бара

	//Создание этого виджета должно происходить после создания всех остальных
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
void ISListBaseForm::DoubleClickedTable(const QModelIndex &ModelIndex)
{
	Q_UNUSED(ModelIndex);
	if (ShowOnly)
	{
		return;
	}
	
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
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
		connect(ObjectFormBase, &ISObjectFormBase::Close, this, &ISListBaseForm::ClosingObjectForm);
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
	QueryModel->SetOrderField(MetaTable->Alias + '_' + FieldName.toLower(), FieldName, Order);
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
	ListIndicatorWidget->setVisible(!SqlModel->rowCount());
	if (SqlModel->rowCount())
	{
		ListIndicatorWidget->SetPixmap(QPixmap());
		ListIndicatorWidget->setCursor(CURSOR_WAIT);
		ListIndicatorWidget->setToolTip(QString());
	}
	else
	{
		ListIndicatorWidget->SetPixmap(BUFFER_ICONS("LabelNoDataTable").pixmap(ISDefines::Gui::SIZE_32_32));
		ListIndicatorWidget->SetText(LANG("NoData"));
		ListIndicatorWidget->setCursor(CURSOR_WHATS_THIS);
		ListIndicatorWidget->setToolTip(LANG("ClickCreateFromCreateObject"));
	}
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

	ActionSetEnabled(ISNamespace::AT_Delete, SelectedRows);
	ActionSetEnabled(ISNamespace::AT_DeleteCascade, SelectedRows);
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

	//Если данных в модели нет и загрузка была после поиска - показываем соответствующую надпись и меняем флаг поиска
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
	if (ShowOnly)
	{
		ActionSetVisible(ISNamespace::AT_Create, false);
		ActionSetVisible(ISNamespace::AT_CreateCopy, false);
		ActionSetVisible(ISNamespace::AT_Edit, false);
		ActionSetVisible(ISNamespace::AT_Delete, false);
		ActionSetVisible(ISNamespace::AT_DeleteCascade, false);
	}
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
void ISListBaseForm::InsertAction(QAction *ActionBefore, QAction *ActionAfter, bool AddingToActionGroup)
{
	ToolBar->insertAction(ActionAfter, ActionBefore);
	TableView->insertAction(ActionAfter, ActionBefore);

	if (AddingToActionGroup)
	{
		ActionObjectGroup->addAction(ActionBefore);
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
QStatusBar* ISListBaseForm::GetStatusBar()
{
	return StatusBar;
}
//-----------------------------------------------------------------------------
QHBoxLayout* ISListBaseForm::GetLayoutTableView()
{
	return LayoutTableView;
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
ISSqlModelCore* ISListBaseForm::GetSqlModel()
{
	return SqlModel;
}
//-----------------------------------------------------------------------------
ISModelThreadQuery* ISListBaseForm::GetModelThread()
{
	return ModelThreadQuery;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateDelegates()
{
	if (DelegatesCreated) //Если делегаты уже созданы - выходить из метода
	{
		return;
	}

	for (int i = 0; i < SqlModel->columnCount(); ++i) //Обход полей
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
	int Index = SqlModel->GetFieldIndex(FieldName);
	IS_ASSERT(Index != -1, QString("Not found field \"%1\"").arg(FieldName));
	TableView->hideColumn(Index);
	FieldResized(true);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowField(const QString &FieldName)
{
	FieldResized(false);
	int Index = SqlModel->GetFieldIndex(FieldName);
	IS_ASSERT(Index != -1, QString("Not found field \"%1\"").arg(FieldName));
	TableView->showColumn(Index);
	FieldResized(true);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SetShowOnly(bool show_only)
{
	ShowOnly = show_only;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Period()
{
	ISPeriodForm PeriodForm;
	PeriodForm.SetRange(QueryModel->GetPeriodType(), QueryModel->GetPeriod());
	if (PeriodForm.Exec())
	{
		QDate StartDate = PeriodForm.GetRange().BeginValue.toDate();
		QDate EndDate = PeriodForm.GetRange().EndValue.toDate();

		switch (PeriodForm.GetPeriodType())
		{
		case ISNamespace::PT_CreationDate: LabelPeriod->setText(LANG("PeriodLabelCreate").arg(StartDate.toString(FORMAT_DATE_V2)).arg(EndDate.toString(FORMAT_DATE_V2))); break;
		case ISNamespace::PT_UpdationDate: LabelPeriod->setText(LANG("PeriodLabelUpdate").arg(StartDate.toString(FORMAT_DATE_V2)).arg(EndDate.toString(FORMAT_DATE_V2))); break;
		}

		LabelPeriod->setVisible(true);
		QueryModel->SetPeriod(PeriodForm.GetPeriodType(), PeriodForm.GetRange());
		ActionPeriodClear->setEnabled(true);
		Update();
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::PeriodClear()
{
	LabelPeriod->setVisible(false);
	LabelPeriod->clear();
	ActionPeriodClear->setEnabled(false);
	QueryModel->ClearPeriod();
	Update();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ResizeColumnsToContents()
{
	FieldResized(false);
	for (int i = 0, c = SqlModel->columnCount(); i < c; ++i)
	{
		QString FieldName = SqlModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString();
		int ColumnSize = ISColumnSizer::Instance().GetColumnSize(MetaTable->Name, FieldName);
		if (ColumnSize) //Если есть размер столбца в памяти, использовать его
		{
			TableView->setColumnWidth(i, ColumnSize);
		}
	}
	FieldResized(true);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::AddWidgetToBottom(QWidget *Widget)
{
	StatusBar->addWidget(Widget);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SetVisibleBottom(bool Visible)
{
	StatusBar->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ClosingObjectForm()
{
	TableView->setFocus();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SelectRowObject(const ISVectorInt &Objects)
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
void ISListBaseForm::SearchFast(const QString &SearchValue)
{
	ISQueryPool::Instance().AddQuery(QI_SEARCH_FAST,
	{
		{ ":Value", SearchValue }
	});

	QString PreparedSearchValue = SearchValue.toLower(); //Преобразование поискового запроса в нижний регистр (ОБЯЗАТЕЛЬНО!!!)
	if (PreparedSearchValue.length()) //Если пользователь ввел поисковое значение
	{
		EditSearch->setAccessibleName(PreparedSearchValue); //Запомнить предыдущее поисковое значение
		QString WhereText = "lower(concat(";

		for (int Column = 0; Column < SqlModel->columnCount(); ++Column) //Обход полей записи
		{
			PMetaField *MetaField = SqlModel->GetField(Column);
			if (!ISMetaData::Instance().GetSearch(MetaField->Type))
			{
				continue;
			}

			if (!MetaField->QueryText.isEmpty()) //Если поле является виртуальным
			{
				WhereText += '(' + MetaField->QueryText + ')';
			}
			else //Стандартное мета-поле
			{
				if (MetaField->Foreign) //Если на поле установлен внешний ключ
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
				else //Поле без внешнего ключа
				{
					WhereText += MetaTable->Alias + '_' + MetaField->Name;
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
void ISListBaseForm::ActionSetVisible(ISNamespace::ActionType action_type, bool visible)
{
	QAction *Action = GetAction(action_type);
	if (Action)
	{
		Action->setVisible(visible);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ActionSetEnabled(ISNamespace::ActionType action_type, bool enabled)
{
	QAction *Action = GetAction(action_type);
	if (Action)
	{
		Action->setEnabled(enabled);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ActionSetText(ISNamespace::ActionType action_type, const QString &text)
{
	QAction *Action = GetAction(action_type);
	if (Action)
	{
		Action->setText(text);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ActionSetToolTip(ISNamespace::ActionType action_type, const QString &tool_tip)
{
	QAction *Action = GetAction(action_type);
	if (Action)
	{
		Action->setToolTip(tool_tip);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadStarted()
{
	ISGui::SetWaitGlobalCursor(true);

	ListIndicatorWidget->SetPixmap(QPixmap());
	ListIndicatorWidget->SetVisibleAnimation(true);
	ListIndicatorWidget->SetText(LANG("LoadDataPleceWait"));
	ListIndicatorWidget->setVisible(true);

	LabelRowCount->setText(QString("%1: %2...").arg(LANG("RecordsCount")).arg(LANG("Calculated"))); //Изменение значения в надписе "Записей"
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

	CreateDelegates();
	LoadDataAfterEvent();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadErrorConnection(const QSqlError &SqlError)
{
	ISGui::SetWaitGlobalCursor(false);

	ListIndicatorWidget->hide();
	ToolBar->setEnabled(true);

	ISLOGGER_W(SqlError.text());
	ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionLoadingData") + "\n\n" + SqlError.text());
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText)
{
	ISGui::SetWaitGlobalCursor(false);

	ListIndicatorWidget->hide();
	ToolBar->setEnabled(true);

	ISLOGGER_W(SqlError.text());
	ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionLoadingData") + "\n\n" + SqlError.text(), QueryText);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Create()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Create").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->Name, 0, parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, this, &ISListBaseForm::ClosingObjectForm);
	emit AddFormFromTab(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateCopy()
{
	if (CheckIsSystemObject())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.NotEdit"));
		return;
	}

	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.CreateCopy").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::OFT_Copy, MetaTable->Name, GetObjectID(), parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, this, &ISListBaseForm::ClosingObjectForm);
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

	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, MetaTable->Name, GetObjectID(), parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, static_cast<void(ISListBaseForm::*)(int)>(&ISListBaseForm::SetSelectObjectAfterUpdate));
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, this, &ISListBaseForm::ClosingObjectForm);
	emit AddFormFromTab(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Update()
{
	SelectObjectAfterUpdate = GetSelectedIDs(); //Запоминаем выделенные записи перед обновлением таблицы
	IsLoadingData = true; //Устанавливаем флаг что сейчас происходит загрузка данных
	SqlModel->Clear();	 //Чистим модель

	QueryModel->SetParentFilter(GetParentObjectID(), GetParentFilterField());
	ModelThreadQuery->Execute(QueryModel->GetQueryText(), QueryModel->GetConditions());

	LabelSelectedRow->setVisible(false);
	LabelSelectedRow->clear();

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_PAGE_NAVIGATION))
	{
		PageNavigation->SetRowCount(ISDatabaseHelper::GetCountRows(MetaTable->Name, MetaTable->Alias));
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Delete()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	bool show_is_deleted = QueryModel->GetVisibleIsDeleted();
	QString ErrorString;
	ISVectorInt VectorInt = GetSelectedIDs();
	if (VectorInt.size() == 1) //Выбрана одна запись
	{
		if (CheckIsSystemObject()) //Если запись системная - выдаём предупреждение и выходим из функции
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.NotDelete"));
			return;
		}

		if (ISMessageBox::ShowQuestion(this, show_is_deleted ? LANG("Message.Question.RecoveryObjectSelected") : LANG("Message.Question.DeleteSelectedRecord")))
		{
			if (ISCore::SetIsDeletedObject(!show_is_deleted, MetaTable, VectorInt.front(), ErrorString)) //Если восстановление прошло успешно, обновить таблицу
			{
				SqlModel->RemoveRecord(GetCurrentRowIndex());
				if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM))
				{
					ISPopupMessage::ShowNotification(show_is_deleted ? LANG("NotificationForm.Title.Recovery") : LANG("NotificationForm.Title.Deleted"));
				}
			}
			else
			{
				ISMessageBox::ShowCritical(this, show_is_deleted ? LANG("Message.Error.SetNotIsDeletedObject") : LANG("Message.Error.SetIsDeletedObject"), ErrorString);
			}
		}
	}
	else //Выбрано несколько записей
	{
		if (ISMessageBox::ShowQuestion(this, show_is_deleted ? LANG("Message.Objects.Recovery") : LANG("Message.Objects.Delete").arg(VectorInt.size())))
		{
			if (ISCore::SetIsDeletedObjects(!show_is_deleted, MetaTable, VectorInt, ErrorString))
			{
				Update();
				if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM))
				{
					ISPopupMessage::ShowNotification(show_is_deleted ? LANG("NotificationForm.Title.Recoverys") : LANG("NotificationForm.Title.Deleteds"));
				}
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.SetIsDeletedObject"), ErrorString);
			}
		}
	}
	VisibleIndicatorWidget();
}
//-----------------------------------------------------------------------------
bool ISListBaseForm::DeleteCascade()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return false;
	}

	QString ErrorString;
	bool Result = true;
	ISVectorInt VectorInt = GetSelectedIDs();
	if (VectorInt.size() == 1) //Если удаляется одна запись
	{
		if (CheckIsSystemObject()) //Если запись является системной - выходим из функции
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.NotDelete"));
			return false;
		}
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Object.Delete.Cascade"), LANG("Message.Object.Delete.Cascade.Help")))
		{
			int ObjectID = VectorInt.front();
			Result = ISCore::DeleteCascadeObject(MetaTable, ObjectID, ErrorString);
			if (Result)
			{
				if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM))
				{
					ISPopupMessage::ShowNotification(LANG("NotificationForm.Title.Deleted.Cascade").arg(ObjectID));
				}
				ISProtocol::DeleteCascadeObject(MetaTable->Name, MetaTable->LocalListName, GetObjectID());
				Update();
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.CascadeDeleteObject"), ErrorString);
			}
			return Result;
		}
	}
	else //Удаляется несколько записей
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Objects.Delete.Cascade").arg(VectorInt.size()), LANG("Message.Object.Delete.Cascade.Help")))
		{
			ISProgressForm ProgressForm((int)VectorInt.size(), LANG("DeletingCascadeObjects"), this);
			ProgressForm.show();

			for (int ObjectID : VectorInt)
			{
				ProgressForm.IncrementValue();
				Result = ISCore::DeleteCascadeObject(MetaTable, ObjectID, ErrorString);
				if (Result) //Если запись удалена - протоколируем
				{
					ISProtocol::DeleteCascadeObject(MetaTable->Name, MetaTable->LocalListName, GetObjectID());
				}
				else //Не удалось удалить запись - выходим из цикла
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotDeleteCascadeRecord").arg(ObjectID), ErrorString);
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
	return Result;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowActual()
{
	GetAction(ISNamespace::AT_ShowActual)->setChecked(true);
	GetAction(ISNamespace::AT_ShowDeleted)->setChecked(false);
	QueryModel->SetVisibleIsDeleted(false);
	Update();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowDeleted()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_IS_DELETED))
	{
		GetAction(ISNamespace::AT_ShowActual)->setChecked(true);
		GetAction(ISNamespace::AT_ShowDeleted)->setChecked(false);
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.IsDeleted").arg(MetaTable->LocalListName));
		return;
	}

	GetAction(ISNamespace::AT_ShowActual)->setChecked(false);
	GetAction(ISNamespace::AT_ShowDeleted)->setChecked(true);
	QueryModel->SetVisibleIsDeleted(!QueryModel->GetVisibleIsDeleted());
	Update();
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

			ActionSetEnabled(ISNamespace::AT_SearchClear, true);
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
	ActionSetEnabled(ISNamespace::AT_SearchClear, false);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Export()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_IS_DELETED))
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

			Result = PrintingBase->FillTemplate(); //Заполнение шаблона данными
			if (Result)
			{
				if (PrintListForm.GetPreview()) //Если выбран предварительный просмотр
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
void ISListBaseForm::ShowSystemInfo()
{
	ISGui::ShowSystemInfoRecord(MetaTable, GetObjectID());
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
void ISListBaseForm::CreateActions()
{
	//Создать
	QAction *ActionCreate = ISControls::CreateActionCreate(this);
	ActionCreate->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	connect(ActionCreate, &QAction::triggered, this, &ISListBaseForm::Create);
	Actions[ISNamespace::AT_Create] = ActionCreate;

	//Создать копию
	QAction *ActionCreateCopy = ISControls::CreateActionCreateCopy(this);
	connect(ActionCreateCopy, &QAction::triggered, this, &ISListBaseForm::CreateCopy);
	Actions[ISNamespace::AT_CreateCopy] = ActionCreateCopy;

	//Изменить
	QAction *ActionEdit = ISControls::CreateActionEdit(this);
	connect(ActionEdit, &QAction::triggered, this, &ISListBaseForm::Edit);
	Actions[ISNamespace::AT_Edit] = ActionEdit;

	//Удалить
	QAction *ActionDelete = ISControls::CreateActionDelete(this);
	connect(ActionDelete, &QAction::triggered, this, &ISListBaseForm::Delete);
	Actions[ISNamespace::AT_Delete] = ActionDelete;

	//Удалить каскадом
	QAction *ActionDeleteCascade = ISControls::CreateActionDeleteCascade(this);
	connect(ActionDeleteCascade, &QAction::triggered, this, &ISListBaseForm::DeleteCascade);
	Actions[ISNamespace::AT_DeleteCascade] = ActionDeleteCascade;

	//Обновить
	QAction *ActionUpdate = ISControls::CreateActionUpdate(this);
	connect(ActionUpdate, &QAction::triggered, this, &ISListBaseForm::Update);
	Actions[ISNamespace::AT_Update] = ActionUpdate;

	//Показывать актуальные записи
	QAction *ActionShowActual = new QAction(BUFFER_ICONS("ShowActual"), LANG("ListForm.ShowActual"), this);
	ActionShowActual->setCheckable(true);
	ActionShowActual->setChecked(true);
	ActionShowActual->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F1));
	connect(ActionShowActual, &QAction::triggered, this, &ISListBaseForm::ShowActual);
	Actions[ISNamespace::AT_ShowActual] = ActionShowActual;

	//Показывать удаленные записи
	QAction *ActionShowDeleted = new QAction(BUFFER_ICONS("ShowDeleted"), LANG("ListForm.ShowDeleted"), this);
	ActionShowDeleted->setCheckable(true);
	ActionShowDeleted->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F2));
	connect(ActionShowDeleted, &QAction::triggered, this, &ISListBaseForm::ShowDeleted);
	Actions[ISNamespace::AT_ShowDeleted] = ActionShowDeleted;

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

	//Печать
	QAction *ActionPrint = ISControls::CreateActionPrint(this);
	ActionPrint->setVisible(ISPrintingEntity::Instance().GetCountReports(MetaTable->Name));
	connect(ActionPrint, &QAction::triggered, this, &ISListBaseForm::Print);
	Actions[ISNamespace::AT_Print] = ActionPrint;

	//Избранное
	QAction *ActionFavorites = new QAction(BUFFER_ICONS("Favorites"), LANG("Favorites"), this);
	connect(ActionFavorites, &QAction::triggered, this, &ISListBaseForm::ShowFavorites);
	Actions[ISNamespace::AT_Favorites] = ActionFavorites;

	//Системная информация
	QAction *ActionSystemInformation = ISControls::CreateActionRecordInformartion(this);
	connect(ActionSystemInformation, &QAction::triggered, this, &ISListBaseForm::ShowSystemInfo);
	Actions[ISNamespace::AT_SystemInfo] = ActionSystemInformation;

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
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateSpecialActions()
{
	//Сортировка по умолчанию
	QAction *ActionSortDefault = new QAction(BUFFER_ICONS("DefaultSorting"), LANG("DefaultSorting"), this);
	connect(ActionSortDefault, &QAction::triggered, this, &ISListBaseForm::SortingDefault);
	ActionsSpecial.emplace(ISNamespace::AST_SortDefault, ActionSortDefault);

	//Примечание
	QAction *ActionNoteObject = ISControls::CreateActionNoteObject(this);
	connect(ActionNoteObject, &QAction::triggered, this, &ISListBaseForm::NoteObject);
	ActionsSpecial.emplace(ISNamespace::AST_Note, ActionNoteObject);

	//Автоподбор ширины
	QAction *ActionResizeFromContent = new QAction(this);
	ActionResizeFromContent->setText(LANG("AutoFitColumnWidth"));
	ActionResizeFromContent->setToolTip(LANG("AutoFitColumnWidth"));
	ActionResizeFromContent->setIcon(BUFFER_ICONS("AutoFitColumnWidth"));
	connect(ActionResizeFromContent, &QAction::triggered, this, &ISListBaseForm::AutoFitColumnWidth);
	ActionsSpecial.emplace(ISNamespace::AST_ResizeFromContent, ActionResizeFromContent);

	//Сброс ширины колонок
	QAction *ActionResetWidthColumn = new QAction(this);
	ActionResetWidthColumn->setText(LANG("ResetWidthColumn"));
	ActionResetWidthColumn->setToolTip(LANG("ResetWidthColumn"));
	connect(ActionResetWidthColumn, &QAction::triggered, this, &ISListBaseForm::ResetWidthColumn);
	ActionsSpecial.emplace(ISNamespace::AST_ResetWidthColumn, ActionResetWidthColumn);

	ActionObjectGroup->addAction(ActionNoteObject);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateToolBar()
{
	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	ToolBar->setEnabled(false);
	GetMainLayout()->addWidget(ToolBar);

	ToolBar->addAction(GetAction(ISNamespace::AT_Create));
	ToolBar->addAction(GetAction(ISNamespace::AT_CreateCopy));
	ToolBar->addAction(GetAction(ISNamespace::AT_Edit));
	ToolBar->addAction(GetAction(ISNamespace::AT_Delete));
	ToolBar->addAction(GetAction(ISNamespace::AT_DeleteCascade));
	ToolBar->addAction(GetAction(ISNamespace::AT_Update));
	ToolBar->addAction(GetAction(ISNamespace::AT_Search));
	ToolBar->addAction(GetAction(ISNamespace::AT_SearchClear));
	ToolBar->addAction(GetAction(ISNamespace::AT_Print));

	QAction *ActionPeriod = new QAction(BUFFER_ICONS("Period"), LANG("Period"), ToolBar);
	ActionPeriod->setMenu(new QMenu(ToolBar));
	ToolBar->addAction(ActionPeriod);

	QToolButton *ButtonPeriod = dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionPeriod));
	ButtonPeriod->setPopupMode(QToolButton::InstantPopup);
	ButtonPeriod->setCursor(CURSOR_POINTING_HAND);
	ButtonPeriod->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));

	QAction *ActionPeriodSelect = new QAction(LANG("PeriodSelect"), ActionPeriod);
	connect(ActionPeriodSelect, &QAction::triggered, this, &ISListBaseForm::Period);
	ActionPeriod->menu()->addAction(ActionPeriodSelect);

	ActionPeriod->menu()->addSeparator();

	ActionPeriodClear = new QAction(LANG("PeriodClear"), ActionPeriod);
	ActionPeriodClear->setEnabled(false);
	connect(ActionPeriodClear, &QAction::triggered, this, &ISListBaseForm::PeriodClear);
	ActionPeriod->menu()->addAction(ActionPeriodClear);

	QAction *ActionAdditionally = new QAction(BUFFER_ICONS("AdditionallyActions"), LANG("Additionally"), ToolBar);
	ActionAdditionally->setMenu(new QMenu(ToolBar));
	ToolBar->addAction(ActionAdditionally);

	QToolButton *ButtonAdditionally = dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionAdditionally));
	ButtonAdditionally->setPopupMode(QToolButton::InstantPopup);
	ButtonAdditionally->setCursor(CURSOR_POINTING_HAND);
	ButtonAdditionally->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));

	ActionAdditionally->menu()->addAction(GetAction(ISNamespace::AT_ShowActual));
	ActionAdditionally->menu()->addAction(GetAction(ISNamespace::AT_ShowDeleted));
	ActionAdditionally->menu()->addSeparator();
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
	if (GetAction(ISNamespace::AT_DeleteCascade)) ActionObjectGroup->addAction(GetAction(ISNamespace::AT_DeleteCascade));
	if (GetAction(ISNamespace::AT_SystemInfo)) ActionObjectGroup->addAction(GetAction(ISNamespace::AT_SystemInfo));
	if (GetAction(ISNamespace::AT_Print)) ActionObjectGroup->addAction(GetAction(ISNamespace::AT_Print));
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateTableView()
{
	LayoutTableView = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTableView);

	TableView = new ISBaseTableView(this);
	TableView->SetCornerText(LANG("Reduction.SerialNumber"));
	TableView->SetCornerToolTip(LANG("OrdinalNumber"));
	connect(TableView, &ISBaseTableView::customContextMenuRequested, this, &ISListBaseForm::ShowContextMenu);
	connect(TableView, &ISBaseTableView::CornerClicked, this, &ISListBaseForm::CornerButtonClicked);
	connect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISListBaseForm::SortingChanged);
	FieldResized(true);
	LayoutTableView->addWidget(TableView);

	connect(TableView, &ISBaseTableView::doubleClicked, this, &ISListBaseForm::DoubleClickedTable);

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_MINIMIZEHEIGHTROWS))
	{
		TableView->verticalHeader()->setDefaultSectionSize(19);
	}

	if (SETTING_BOOL(CONST_UID_SETTING_TABLE_SCROLL_SELECTION))
	{
		TableView->SetSelectionScroll(true);
		connect(TableView, &ISBaseTableView::WheelUp, this, &ISListBaseForm::NavigationSelectPreviousRecord);
		connect(TableView, &ISBaseTableView::WheelDown, this, &ISListBaseForm::NavigationSelectNextRecord);
	}

	TableView->setAlternatingRowColors(SETTING_BOOL(CONST_UID_SETTING_TABLES_HIGHLIGHTINGODDROWS));
	TableView->horizontalHeader()->setVisible(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWHORIZONTALHEADER));
	TableView->SetVisibleVerticalHeader(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWVERTICALHEADER));
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateContextMenu()
{
	ContextMenu = new QMenu(this);
	if (GetAction(ISNamespace::AT_Create)) ContextMenu->addAction(GetAction(ISNamespace::AT_Create));
	if (GetAction(ISNamespace::AT_CreateCopy)) ContextMenu->addAction(GetAction(ISNamespace::AT_CreateCopy));
	if (GetAction(ISNamespace::AT_Edit)) ContextMenu->addAction(GetAction(ISNamespace::AT_Edit));
	if (GetAction(ISNamespace::AT_Delete)) ContextMenu->addAction(GetAction(ISNamespace::AT_Delete));
	if (GetAction(ISNamespace::AT_DeleteCascade)) ContextMenu->addAction(GetAction(ISNamespace::AT_DeleteCascade));
	if (GetAction(ISNamespace::AT_Update)) ContextMenu->addAction(GetAction(ISNamespace::AT_Update));
	if (GetAction(ISNamespace::AT_SystemInfo)) ContextMenu->addAction(GetAction(ISNamespace::AT_SystemInfo));
	ContextMenu->addAction(GetSpecialAction(ISNamespace::AST_Note));
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateModels()
{
	if (!MetaTable->SqlModel.isEmpty()) //Если в мета-данных таблицы указана пользовательская модель, создавать её
	{
		SqlModel = ISAlgorithm::CreatePointer<ISSqlModelCore *>(MetaTable->SqlModel, Q_ARG(PMetaTable *, MetaTable), Q_ARG(QObject *, TableView));
	}
	else //Модель в мета-данных таблицы не указана, создавать стандартную (базовую)
	{
		SqlModel = new ISSqlModelCore(MetaTable, TableView);
	}

	SqlModel->FillColumns();
	SqlModel->SetShowToolTip(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWTOOLTIP));

	QueryModel = new ISQueryModel(MetaTable, ISNamespace::QMT_List, this);

	ISSortingMetaTable *MetaSorting = ISSortingBuffer::Instance().GetSorting(MetaTable->Name);
	if (MetaSorting) //Если сортировка для этой таблицы уже существует, использовать её
	{
		SqlModel->SetSorting(MetaSorting->FieldName, MetaSorting->Order);
		QueryModel->SetOrderField(MetaTable->Alias + '_' + MetaSorting->FieldName.toLower(), MetaSorting->FieldName, MetaSorting->Order);
	}

	TableView->setModel(SqlModel);
	connect(TableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ISListBaseForm::SelectedRowEvent);//Это соединение обязательно должно быть после присваивания модели к QTableView

	//Скрытие системных полей
	if (!SETTING_BOOL(CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID))
	{
		HideField("ID");
	}
	HideField("IsDeleted");
	HideField("IsSystem");

	ModelThreadQuery = new ISModelThreadQuery(this);
	connect(ModelThreadQuery, &ISModelThreadQuery::Started, this, &ISListBaseForm::ModelThreadStarted);
	connect(ModelThreadQuery, &ISModelThreadQuery::Finished, this, &ISListBaseForm::ModelThreadFinished);
	connect(ModelThreadQuery, &ISModelThreadQuery::ExecutedQuery, this, &ISListBaseForm::ModelThreadLoadingData);
	connect(ModelThreadQuery, &ISModelThreadQuery::Results, SqlModel, &ISSqlModelCore::SetRecords);
	connect(ModelThreadQuery, &ISModelThreadQuery::ErrorConnection, this, &ISListBaseForm::ModelThreadErrorConnection);
	connect(ModelThreadQuery, &ISModelThreadQuery::ErrorQuery, this, &ISListBaseForm::ModelThreadErrorQuery);
	ModelThreadQuery->start(QThread::TimeCriticalPriority); //Запуск потока
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateStatusBar()
{
	StatusBar = new QStatusBar(this);
	StatusBar->setSizeGripEnabled(false);
	GetMainLayout()->addWidget(StatusBar);

	LabelRowCount = new QLabel(StatusBar);
	LabelRowCount->setVisible(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWCOUNTRECORD));
	LabelRowCount->setText(LANG("RecordsCount") + ": -");
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

	LabelPeriod = new QLabel(StatusBar);
	LabelPeriod->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LabelPeriod->setVisible(false);
	StatusBar->addWidget(LabelPeriod);

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

	QAction *ActionSearchFocus = new QAction(this);
	ActionSearchFocus->setShortcut(Qt::Key_F8);
	connect(ActionSearchFocus, &QAction::triggered, EditSearch, &ISSearchEdit::SetFocus);
	addAction(ActionSearchFocus);
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
