#include "ISListBaseForm.h"
#include "ISDefinesGui.h"
#include "ISAssert.h"
#include "ISCore.h"
#include "ISSettings.h"
#include "ISLocalization.h"
#include "ISSortingBuffer.h"
#include "ISProtocol.h"
#include "ISNotificationService.h"
#include "ISExportForm.h"
#include "ISProgressForm.h"
#include "ISLogger.h"
#include "ISProcessForm.h"
#include "ISFavoritesForm.h"
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
#include "ISNoteObjectForm.h"
#include "ISDelegates.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISMetaUser.h"
#include "ISColumnSizer.h"
#include "ISInputDialog.h"
#include "ISExportWorker.h"
#include "ISNotifySender.h"
#include "ISExportCSV.h"
#include "ISExportHTML.h"
#include "ISExportDBF.h"
#include "ISExportXML.h"
#include "ISPeriodForm.h"
#include "ISQueryPool.h"
#include "ISGui.h"
#include "ISException.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
#include "ISPrintingWord.h"
#endif
//-----------------------------------------------------------------------------
static QString QI_SHARE_RECORD = PREPARE_QUERY("INSERT INTO _chatmessages(chat_message, chat_tablename, chat_objectid) "
											   "VALUES(:Message, :TableName, :ObjectID) "
											   "RETURNING chat_id");
//-----------------------------------------------------------------------------
static QString QI_SEARCH_FAST = PREPARE_QUERY("INSERT INTO _searchfast(srfs_user, srfs_value) "
											  "VALUES(:UserID, :Value)");
//-----------------------------------------------------------------------------
ISListBaseForm::ISListBaseForm(const QString &TableName, QWidget *parent)
	: ISInterfaceMetaForm(parent),
	ActionObjectGroup(new QActionGroup(this)) //Группа действий, остосящихся только к одному объекту
{
	MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
	SqlModel = nullptr;
	PageNavigation = nullptr;
	QueryModel = nullptr;
	SearchForm = nullptr;
	BeginInstallSorting = false;
	SelectObjectAfterUpdate = 0;
	DelegatesCreated = false;
	ShowOnly = MetaTable->ShowOnly;
	IsLoadingData = false;
	SearchFlag = false;

	//Создание действий
	CreateActions();

	//Создание специальных действий
	CreateSpecialActions();

	//Создание тулбара
	CreateToolBar();

	//Создание таблицы
	CreateTableView();

	//Создание контекстного меню
	CreateContextMenu();

	//Создание моделей
	CreateModels();

	//Создание статус-бара
	CreateStatusBar();

	ListIndicatorWidget = new ISListIndicatorWidget(this);
}
//-----------------------------------------------------------------------------
ISListBaseForm::~ISListBaseForm()
{
	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERCOLUMNSIZE)) //Если нужно запоминать размер колонок
	{
		for (int i = 0, c = SqlModel->columnCount(); i < c; ++i)
		{
			QString FieldName = SqlModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString();
			if (!MetaTable->GetField(FieldName)->IsSystem) //Если поле не системное - запомнить размер поля
			{
				ISColumnSizer::Instance().SetColumnSize(MetaTable->Name, FieldName, TableView->columnWidth(i));
			}
		}
	}

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
	int CurrentRow = TableView->selectionModel()->currentIndex().row();
	if (CurrentRow == -1)
	{
		return 0;
	}
	return CurrentRow;
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
	if (ModelIndexList.count())
	{
		for(const QModelIndex &ModelIndex : ModelIndexList)
		{
			VectorInt.emplace_back(GetObjectID(ModelIndex.row()));
		}
	}
	else
	{
		ISMessageBox::ShowInformation(this, LANG("Message.Information.SelectAtLeastOneRecord"));
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
void ISListBaseForm::SetSelectObjectAfterUpdate(int object_id)
{
	SelectObjectAfterUpdate = object_id;
}
//-----------------------------------------------------------------------------
PMetaClassTable* ISListBaseForm::GetMetaTable()
{
	return MetaTable;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::DoubleClickedTable(const QModelIndex &ModelIndex)
{
	if (ShowOnly)
	{
		emit DoubleClicked(ModelIndex);
	}
	else
	{
		if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
			return;
		}

		QString EventName = SETTING_STRING(CONST_UID_SETTING_TABLES_DOUBLECLICKEVENT);
		if (EventName == "Window")
		{
			ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, MetaTable->Name, GetObjectID());
			ObjectFormBase->SetParentObjectID(GetParentObjectID());
			connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::SetSelectObjectAfterUpdate);
			connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
			connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
			connect(ObjectFormBase, &ISObjectFormBase::Close, this, &ISListBaseForm::ClosingObjectForm);
			ObjectFormBase->show();
		}
		else if (EventName == "Tab")
		{
			Edit();
		}
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SortingChanged(int LogicalIndex, Qt::SortOrder SortOrder)
{
	QString FieldName = SqlModel->headerData(LogicalIndex, Qt::Horizontal, Qt::UserRole).toString();
	QString OrderField = MetaTable->Alias + '_' + FieldName.toLower();
	QueryModel->SetOrderField(OrderField, FieldName);
	QueryModel->SetOrderSort(SortOrder);
	SqlModel->SetCurrentSorting(LogicalIndex, SortOrder);

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERSORTING))
	{
		ISSortingBuffer::GetInstance().AddSorting(MetaTable->Name, FieldName, SortOrder);
	}

	if (BeginInstallSorting)
	{
		Update();
	}

	//Отсоединение и соединение нужно обязательно, иначе в этом методе произойдет рекурсия
	disconnect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISListBaseForm::SortingChanged);
	TableView->horizontalHeader()->setSortIndicator(LogicalIndex, SortOrder);
	connect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISListBaseForm::SortingChanged);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::SortingDefault()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.SortingDefault")))
	{
		TableView->horizontalHeader()->sortIndicatorChanged(0, Qt::AscendingOrder);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::HideSystemFields()
{
	if (!SETTING_BOOL(CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID))
	{
		HideField("ID");
	}
	HideField("IsDeleted");
	HideField("IsSystem");
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

	if (SqlModel->rowCount())
	{
		LabelSearchResult->setVisible(false);
	}
	else
	{
		if (SearchFlag)
		{
			SearchFlag = false;
			LabelSearchResult->setVisible(true);
		}
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
		QRect Rect = TableView->frameGeometry();
		QPoint CenterPoint = Rect.center();
		CenterPoint.setX(CenterPoint.x() - (ListIndicatorWidget->width() / 2));
		CenterPoint.setY(CenterPoint.y() - (ListIndicatorWidget->height() / 2));
		ListIndicatorWidget->move(CenterPoint);
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
	return Actions.value(action_type);
}
//-----------------------------------------------------------------------------
QAction* ISListBaseForm::GetSpecialAction(ISNamespace::ActionSpecialType action_special)
{
	return ActionsSpecial.value(action_special);
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
		PMetaClassField *MetaField = MetaTable->GetField(HeaderData);

		QAbstractItemDelegate *AbstractItemDelegate = TableView->itemDelegateForColumn(i);
		if (!AbstractItemDelegate)
		{
			switch (MetaField->Type)
			{
			case ISNamespace::FT_Bool: AbstractItemDelegate = new ISDelegateBoolean(TableView); break;
			case ISNamespace::FT_Image: AbstractItemDelegate = new ISDelegateImage(TableView); break;
			case ISNamespace::FT_Color: AbstractItemDelegate = new ISDelegateColor(TableView); break;
			case ISNamespace::FT_CallDetails: AbstractItemDelegate = new ISDelegateCallDetails(TableView); break;
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
	for (int i = 0; i < SqlModel->columnCount(); ++i)
	{
		QString HeaderData = SqlModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString();
		if (HeaderData == FieldName)
		{
			TableView->hideColumn(i);
			return;
		}
	}
	ISLOGGER_WARNING(QString("Not found field \"%1\" from HideField").arg(FieldName));
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowField(const QString &FieldName)
{
	for (int i = 0; i < SqlModel->columnCount(); ++i)
	{
		QString HeaderData = SqlModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString();
		if (HeaderData == FieldName)
		{
			TableView->showColumn(i);
			return;
		}
	}
	ISLOGGER_WARNING(QString("Not found field \"%1\" from ShowField").arg(FieldName));
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
		Update();
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::PeriodClear()
{
	LabelPeriod->setVisible(false);
	LabelPeriod->clear();

	QueryModel->ClearPeriod();
	Update();
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ResizeColumnsToContents()
{
	for (int i = 0, c = SqlModel->columnCount(); i < c; ++i)
	{
		QString FieldName = SqlModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString();
		int ColumnSize = ISColumnSizer::Instance().GetColumnSize(MetaTable->Name, FieldName);
		if (ColumnSize) //Если есть размер столбца в памяти, использовать его
		{
			TableView->setColumnWidth(i, ColumnSize);
		}
	}
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
void ISListBaseForm::SelectRowObject(int object_id)
{
	if (object_id)
	{
		int RowIndex = GetRowIndex(object_id);
		if (RowIndex != -1)
		{
			SelectRowIndex(RowIndex);
			SetSelectObjectAfterUpdate(-1);
		}
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
	QVariantMap VariantMap;
	VariantMap.insert(":UserID", ISMetaUser::GetInstance().GetData()->ID);
	VariantMap.insert(":Value", SearchValue);
	ISQueryPool::GetInstance().AddQuery(QI_SEARCH_FAST, VariantMap);

	QString PreparedSearchValue = SearchValue.toLower(); //Преобразование поискового запроса в нижний регистр (ОБЯЗАТЕЛЬНО!!!)
	if (PreparedSearchValue.length()) //Если пользователь ввел поисковое значение
	{
		EditSearch->setAccessibleName(PreparedSearchValue); //Запомнить предыдущее поисковое значение
		QString WhereText = "lower(concat(";

		for (int Column = 0; Column < SqlModel->columnCount(); ++Column) //Обход полей записи
		{
			PMetaClassField *MetaField = SqlModel->GetField(Column);
			if (!ISMetaData::GetInstanse().GetSearch(MetaField->Type))
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
					PMetaClassTable *MetaForeignTable = ISMetaData::GetInstanse().GetMetaTable(MetaField->Foreign->ForeignClass);
					WhereText += "(SELECT concat(";
					for (const QString &FieldName : MetaField->Foreign->ForeignViewNameField.split(';'))
					{
						WhereText += MetaForeignTable->Alias + '_' + FieldName + ", ";
					}
					ISSystem::RemoveLastSymbolFromString(WhereText, 2);
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

		ISSystem::RemoveLastSymbolFromString(WhereText, 7);
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
	ListIndicatorWidget->SetText(LANG("LoadDataPleceWait") + "...");
	ListIndicatorWidget->setVisible(true);

	LabelRowCount->setText(QString("%1: %2...").arg(LANG("RecordsCount")).arg(LANG("Calculated"))); //Изменение значения в надписе "Записей"
	ToolBar->setEnabled(false);
	SetEnabledPageNavigation(false);
	EditSearch->setEnabled(false);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadLoadingData()
{
	ListIndicatorWidget->SetText(LANG("FillTableData") + "...");
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

	if (SqlError.type() == QSqlError::ConnectionError)
	{
		throw ISExceptionConnectionDB(SqlError.text());
	}
	else
	{
		ISLOGGER_WARNING(SqlError.text());
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionLoadingData") + "\n\n" + SqlError.text());
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText)
{
	ISGui::SetWaitGlobalCursor(false);

	ListIndicatorWidget->hide();
	ToolBar->setEnabled(true);

	if (SqlError.type() == QSqlError::ConnectionError)
	{
		throw ISExceptionConnectionDB(SqlError.text());
	}
	else
	{
		ISLOGGER_WARNING(SqlError.text());
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionLoadingData") + "\n\n" + SqlError.text(), QueryText);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Create()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Create").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->Name, 0, parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::SetSelectObjectAfterUpdate);
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

	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.CreateCopy").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::OFT_Copy, MetaTable->Name, GetObjectID(), parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::SetSelectObjectAfterUpdate);
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

	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, MetaTable->Name, GetObjectID(), parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::SetSelectObjectAfterUpdate);
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, this, &ISListBaseForm::ClosingObjectForm);
	emit AddFormFromTab(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Update()
{
	if (!BeginInstallSorting) //Если первичная сортировка ещё не была установлена (первая загрузка данных в таблицу)
	{
		ISSortingMetaTable *MetaSorting = ISSortingBuffer::GetInstance().GetSorting(MetaTable->Name);
		if (MetaSorting) //Если сортировка для этой таблицы уже существует, использовать её
		{
			SortingChanged(SqlModel->GetFieldIndex(MetaSorting->FieldName), static_cast<Qt::SortOrder>(MetaSorting->Sorting));
		}
		else //Сортировка не существует, использовать стандартную по коду (от меньшего к большему)
		{
			SortingChanged(0, Qt::AscendingOrder);
		}

		QueryModel->SetParentObjectIDClassFilter(GetParentObjectID());
		BeginInstallSorting = true;
	}

	IsLoadingData = true;
	SqlModel->Clear();
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
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	ISVectorInt VectorInt = GetSelectedIDs();
	if (VectorInt.size() == 1) //Если помечается на удаление одна запись
	{
		if (CheckIsSystemObject())
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.NotDelete"));
			return;
		}

		if (GetCurrentRecordValue("IsDeleted").toBool()) //Если объект удаленный - восстановить
		{
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.RecoveryObjectSelected")))
			{
				if (ISCore::DeleteOrRecoveryObject(ISNamespace::DRO_Recovery, MetaTable->Name, MetaTable->Alias, VectorInt.at(0), MetaTable->LocalListName)) //Если восстановление прошло успешно, обновить таблицу
				{
					ISNotificationService::ShowNotification(ISNamespace::NotificationFormType::NFT_Recovery, MetaTable->LocalName);
					if (QueryModel->GetVisibleIsDeleted())
					{
						SqlModel->setData(SqlModel->index(GetCurrentRowIndex(), SqlModel->GetFieldIndex("IsDeleted")), false);
					}
					else
					{
						SqlModel->RemoveRecord(GetCurrentRowIndex());
					}
				}
			}
		}
		else //Если объект не удаленный - удалить
		{
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteSelectedRecord")))
			{
				if (ISCore::DeleteOrRecoveryObject(ISNamespace::DRO_Delete, MetaTable->Name, MetaTable->Alias, VectorInt.at(0), MetaTable->LocalListName)) //Если удаление прошло успешно, обновить таблицу
				{
					ISNotificationService::ShowNotification(ISNamespace::NotificationFormType::NFT_Delete, MetaTable->LocalName);
					if (QueryModel->GetVisibleIsDeleted())
					{
						SqlModel->setData(SqlModel->index(GetCurrentRowIndex(), SqlModel->GetFieldIndex("IsDeleted")), true);
					}
					else
					{
						SqlModel->RemoveRecord(GetCurrentRowIndex());
					}
				}
			}
		}
	}
	else //Помечается на удаление несколько записей
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Objects.Delete").arg(VectorInt.size())))
		{
			ISProgressForm ProgressForm(0, VectorInt.size(), this);
			ProgressForm.show();
			ProgressForm.SetText(LANG("DeletingObjects").arg(0).arg(VectorInt.size()) + "...");

			for (int i = 0; i < VectorInt.size(); ++i)
			{
				ProgressForm.SetText(LANG("DeletingObjects").arg(i + 1).arg(VectorInt.size()) + "...");
				if (ProgressForm.wasCanceled())
				{
					break;
				}

				ISCore::DeleteOrRecoveryObject(ISNamespace::DRO_Delete, MetaTable->Name, MetaTable->Alias, VectorInt.at(i), MetaTable->LocalListName);
				ProgressForm.AddOneValue();
			}

			Update();
		}
	}

	VisibleIndicatorWidget();
}
//-----------------------------------------------------------------------------
bool ISListBaseForm::DeleteCascade()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return false;
	}

	ISVectorInt VectorInt = GetSelectedIDs();
	if (VectorInt.size() == 1) //Если удаляется одна запись
	{
		if (CheckIsSystemObject())
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.NotDelete"));
			return false;
		}

		if (ISMessageBox::ShowQuestion(this, LANG("Message.Object.Delete.Cascade"), LANG("Message.Object.Delete.Cascade.Help")))
		{
			int ObjectID = VectorInt.at(0);
			if (ISCore::DeleteCascadeObject(MetaTable->Name, MetaTable->Alias, ObjectID))
			{
				ISNotificationService::ShowNotification(LANG("NotificationForm.Title.Deleted.Cascade").arg(ObjectID));
				ISProtocol::DeleteCascadeObject(MetaTable->Name, MetaTable->LocalListName, GetObjectID());
				Update();

				return true;
			}
		}
	}
	else //Удаляется несколько записей
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Objects.Delete.Cascade").arg(VectorInt.size()), LANG("Message.Object.Delete.Cascade.Help")))
		{
			ISProgressForm ProgressForm(0, VectorInt.size(), this);
			ProgressForm.show();
			ProgressForm.SetText(LANG("DeletingCascadeObjects").arg(0).arg(VectorInt.size()) + "...");

			for (int i = 0; i < VectorInt.size(); ++i)
			{
				ProgressForm.SetText(LANG("DeletingCascadeObjects").arg(i + 1).arg(VectorInt.size()) + "...");
				ProgressForm.AddOneValue();

				int ObjectID = VectorInt.at(i);
				if (ISCore::DeleteCascadeObject(MetaTable->Name, MetaTable->Alias, ObjectID))
				{
					ISProtocol::DeleteCascadeObject(MetaTable->Name, MetaTable->LocalListName, GetObjectID());
				}
				else
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotDeleteCascadeRecord").arg(ObjectID));
					return false;
				}

				if (ProgressForm.wasCanceled())
				{
					break;
				}
			}

			Update();
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowDeleted()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_IS_DELETED))
	{
		GetAction(ISNamespace::AT_ShowDeleted)->setChecked(false);
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.IsDeleted").arg(MetaTable->LocalListName));
		return;
	}

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
	SearchForm->ShowAnimated(false, DURATION_SHOW_HIDE_SEARCH_FORM);
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
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_IS_DELETED))
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
	case ISNamespace::ET_CSV: ExportWorker = new ISExportCSV(this); break;
	case ISNamespace::ET_HTML: ExportWorker = new ISExportHTML(this); break;
	case ISNamespace::ET_DBF: ExportWorker = new ISExportDBF(this); break;
	case ISNamespace::ET_XML: ExportWorker = new ISExportXML(this); break;
	}

	ExportWorker->SetLocalName(MetaTable->LocalListName);
	ExportWorker->SetTableName(MetaTable->Name);
	ExportWorker->SetFields(ExportForm.GetSelectedFields());
	ExportWorker->SetModel(SqlModel);
	ExportWorker->SetHeader(ExportForm.GetHeader());
	ExportWorker->SetSelectedRows(GetSelectedRowIndexes());

	ISProgressForm ProgressForm(0, SqlModel->rowCount(), this);
	ProgressForm.show();
	ProgressForm.SetText(LANG("Export.Process.Prepare") + "...");
	connect(&ProgressForm, &ISProgressForm::canceled, ExportWorker, &ISExportWorker::Cancel);
	connect(ExportWorker, &ISExportWorker::ExportedRow, &ProgressForm, &ISProgressForm::AddOneValue);
	connect(ExportWorker, &ISExportWorker::Message, &ProgressForm, &ISProgressForm::SetText);

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

	delete ExportWorker;
	ExportWorker = nullptr;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Print()
{
	if (!ISPrintingEntity::GetInstance().GetCountReports(MetaTable->Name))
	{
		ISMessageBox::ShowInformation(this, LANG("Message.Information.NotFoundPrintFormFromMetaTable").arg(MetaTable->LocalListName));
		return;
	}

	ISGui::SetWaitGlobalCursor(true);
	ISPrintForm PrintListForm(MetaTable->Name, this);
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
	bool EditPrintForm = ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_REPORT_FORM_EDIT);

	if (MetaReport->Type == ISNamespace::RT_Html)
	{
		PrintingBase = new ISPrintingHtml(MetaReport, GetObjectID(), this);
		PrintingBase->setProperty("PDF", PrintListForm.GetPDF());
		PrintingBase->setProperty("PathPDF", QDir::homePath() + '/' + MetaReport->LocalName);
		PrintingBase->setProperty("EditPreview", EditPrintForm);
	}
	else if (MetaReport->Type == ISNamespace::RT_Word)
	{
#ifdef WIN32
		PrintingBase = new ISPrintingWord(MetaReport, GetObjectID(), this);
#endif
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

	if (PrintingBase)
	{
		delete PrintingBase;
		PrintingBase = nullptr;
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowSystemInfo()
{
	ISGui::ShowSystemInfoRecord(MetaTable, GetObjectID());
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Share()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ShareRecordFromChat")))
	{
		QVariant MessageText = ISInputDialog::GetString(this, LANG("Share"), LANG("InputMessageFromLinkToRecord"));

		ISQuery qShare(QI_SHARE_RECORD);
		qShare.BindValue(":Message", MessageText);
		qShare.BindValue(":TableName", MetaTable->Name);
		qShare.BindValue(":ObjectID", GetObjectID());
		if (qShare.ExecuteFirst())
		{
			int MessageID = qShare.ReadColumn("chat_id").toInt();
			ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_NEW_CHAT_MESSAGE, MessageID, QString(), false);
			ISProtocol::Insert(true, CONST_UID_PROTOCOL_SHARE_RECORD, MetaTable->Name, MetaTable->LocalListName, GetObjectID());
			ISMessageBox::ShowInformation(this, LANG("Message.Information.SharedRecord"));
		}
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::AttachTask()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_ATTACH_OBEJCT_TASK))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.AttachObjectTask"));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.AttachRecordFromTask")))
	{
		int TaskID = ISGui::SelectObject("_Task");
		if (TaskID)
		{
			ISGui::SetWaitGlobalCursor(true);
			if (ISCore::TaskIsAttachedObject(TaskID, MetaTable->Name, GetObjectID())) //Если запись уже прикреплена к выбранной задаче
			{
				ISGui::SetWaitGlobalCursor(false);
				if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DetachObjectTask")))
				{
					ISGui::SetWaitGlobalCursor(true);
					if (ISCore::TaskDetachObject(TaskID, MetaTable->Name, GetObjectID()))
					{
						ISGui::SetWaitGlobalCursor(false);
						ISMessageBox::ShowInformation(this, LANG("Message.Information.DetachedObjectTask"));
					}
				}
			}
			else //Запись не прикреплена к выбранной задаче
			{
				ISGui::SetWaitGlobalCursor(true);
				if (ISCore::TaskAttachObject(TaskID, MetaTable->Name, GetObjectID()))
				{
					ISGui::SetWaitGlobalCursor(false);
					ISProtocol::Insert(true, CONST_UID_PROTOCOL_ATTACH_OBJECT_TASK, MetaTable->Name, MetaTable->LocalListName, GetObjectID());
					ISMessageBox::ShowInformation(this, LANG("Message.Information.AttachedObjectTask"));
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::ShowFavorites()
{
	ISGui::SetWaitGlobalCursor(true);
	ISFavoritesForm *FavoritesForm = new ISFavoritesForm(nullptr, MetaTable);
	FavoritesForm->show();
	connect(FavoritesForm, &ISFavoritesForm::OpenObject, [=](const QString &TableName, int ObjectID)
	{
		Q_UNUSED(TableName);
		ISGui::CreateObjectForm(ISNamespace::OFT_Edit, TableName, ObjectID)->show();
	});
	ISGui::SetWaitGlobalCursor(false);
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
	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_RECORD_NOTE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.RecordNote"));
		return;
	}

	ISGui::SetWaitGlobalCursor(true);
	ISNoteObjectForm NoteObjectForm(MetaTable->Name, GetObjectID());
	ISGui::SetWaitGlobalCursor(false);
	NoteObjectForm.Exec();
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
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ResetWidthColumn")))
	{
		ISGui::SetWaitGlobalCursor(true);
		for (int i = 0; i < SqlModel->columnCount(); ++i)
		{
			TableView->setColumnWidth(i, 100);
		}
		ISGui::SetWaitGlobalCursor(false);
	}
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CopyRecord()
{
	QString Content;
	QSqlRecord SqlRecord = GetCurrentRecord();

	for (int i = 0; i < SqlModel->columnCount(); ++i)
	{
		Content += SqlRecord.value(i).toString() + "\n";
	}

	ISSystem::RemoveLastSymbolFromString(Content);
	QApplication::clipboard()->setText(Content);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateActions()
{
	//Создать
	QAction *ActionCreate = ISControls::CreateActionCreate(this);
	ActionCreate->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	connect(ActionCreate, &QAction::triggered, this, &ISListBaseForm::Create);
	Actions.insert(ISNamespace::AT_Create, ActionCreate);

	//Создать копию
	QAction *ActionCreateCopy = ISControls::CreateActionCreateCopy(this);
	connect(ActionCreateCopy, &QAction::triggered, this, &ISListBaseForm::CreateCopy);
	Actions.insert(ISNamespace::AT_CreateCopy, ActionCreateCopy);

	//Изменить
	QAction *ActionEdit = ISControls::CreateActionEdit(this);
	connect(ActionEdit, &QAction::triggered, this, &ISListBaseForm::Edit);
	Actions.insert(ISNamespace::AT_Edit, ActionEdit);

	//Удалить
	QAction *ActionDelete = ISControls::CreateActionDelete(this);
	connect(ActionDelete, &QAction::triggered, this, &ISListBaseForm::Delete);
	Actions.insert(ISNamespace::AT_Delete, ActionDelete);

	//Удалить каскадом
	QAction *ActionDeleteCascade = ISControls::CreateActionDeleteCascade(this);
	connect(ActionDeleteCascade, &QAction::triggered, this, &ISListBaseForm::DeleteCascade);
	Actions.insert(ISNamespace::AT_DeleteCascade, ActionDeleteCascade);

	//Обновить
	QAction *ActionUpdate = ISControls::CreateActionUpdate(this);
	connect(ActionUpdate, &QAction::triggered, this, &ISListBaseForm::Update);
	Actions.insert(ISNamespace::AT_Update, ActionUpdate);

	//Показывать удаленные записи
	QAction *ActionShowDeleted = ISControls::CreateActionShowDeleted(this);
	connect(ActionShowDeleted, &QAction::triggered, this, &ISListBaseForm::ShowDeleted);
	Actions.insert(ISNamespace::AT_ShowDeleted, ActionShowDeleted);

	//Поиск
	QAction *ActionSearch = ISControls::CreateActionSearch(this);
	connect(ActionSearch, &QAction::triggered, this, &ISListBaseForm::Search);
	Actions.insert(ISNamespace::AT_Search, ActionSearch);

	//Очистка результатов поиска
	QAction *ActionSearchClearResult = ISControls::CreateActionSearchClearResults(this);
	ActionSearchClearResult->setEnabled(false);
	connect(ActionSearchClearResult, &QAction::triggered, this, &ISListBaseForm::SearchClear);
	Actions.insert(ISNamespace::AT_SearchClear, ActionSearchClearResult);

	//Экспорт
	QAction *ActionExport = ISControls::CreateActionExport(this);
	connect(ActionExport, &QAction::triggered, this, &ISListBaseForm::Export);
	Actions.insert(ISNamespace::AT_Export, ActionExport);

	//Печать
	QAction *ActionPrint = ISControls::CreateActionPrint(this);
	connect(ActionPrint, &QAction::triggered, this, &ISListBaseForm::Print);
	Actions.insert(ISNamespace::AT_Print, ActionPrint);

	//Избранное
	QAction *ActionFavorites = ISControls::CreateActionFavorites(this);
	connect(ActionFavorites, &QAction::triggered, this, &ISListBaseForm::ShowFavorites);
	Actions.insert(ISNamespace::AT_Favorites, ActionFavorites);

	//Системная информация
	QAction *ActionSystemInformation = ISControls::CreateActionRecordInformartion(this);
	connect(ActionSystemInformation, &QAction::triggered, this, &ISListBaseForm::ShowSystemInfo);
	Actions.insert(ISNamespace::AT_SystemInfo, ActionSystemInformation);

	//Поделиться
	QAction *ActionShare = ISControls::CreateActionShare(this);
	connect(ActionShare, &QAction::triggered, this, &ISListBaseForm::Share);
	Actions.insert(ISNamespace::AT_Share, ActionShare);

	QAction *ActionAttachTask = ISControls::CreateActionAttachTask(this);
	connect(ActionAttachTask, &QAction::triggered, this, &ISListBaseForm::AttachTask);
	Actions.insert(ISNamespace::AT_AttachTask, ActionAttachTask);

	//Первая запись
	QAction *ActionNavigationBegin = ISControls::CreateActionNavigationBegin(this);
	connect(ActionNavigationBegin, &QAction::triggered, this, &ISListBaseForm::NavigationSelectBeginRecord);
	Actions.insert(ISNamespace::AT_NavigationBegin, ActionNavigationBegin);

	//Предыдущая запись
	QAction *ActionNavigationPrevious = ISControls::CreateActionNavigationPrevious(this);
	connect(ActionNavigationPrevious, &QAction::triggered, this, &ISListBaseForm::NavigationSelectPreviousRecord);
	Actions.insert(ISNamespace::AT_NavigationPrevious, ActionNavigationPrevious);

	//Следующая запись
	QAction *ActionNavigationNext = ISControls::CreateActionNavigationNext(this);
	connect(ActionNavigationNext, &QAction::triggered, this, &ISListBaseForm::NavigationSelectNextRecord);
	Actions.insert(ISNamespace::AT_NavigationNext, ActionNavigationNext);

	//Последняя запись
	QAction *ActionNavigationLast = ISControls::CreateActionNavigationLast(this);
	connect(ActionNavigationLast, &QAction::triggered, this, &ISListBaseForm::NavigationSelectLastRecord);
	Actions.insert(ISNamespace::AT_NavigationLast, ActionNavigationLast);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateSpecialActions()
{
	//Сортировка по умолчанию
	QAction *ActionSortDefault = ISControls::CreateActionSortDefault(this);
	connect(ActionSortDefault, &QAction::triggered, this, &ISListBaseForm::SortingDefault);
	ActionsSpecial.insert(ISNamespace::AST_SortDefault, ActionSortDefault);

	//Примечание
	QAction *ActionNoteObject = ISControls::CreateActionNoteObject(this);
	connect(ActionNoteObject, &QAction::triggered, this, &ISListBaseForm::NoteObject);
	ActionsSpecial.insert(ISNamespace::AST_Note, ActionNoteObject);

	//Автоподбор ширины
	QAction *ActionResizeFromContent = new QAction(this);
	ActionResizeFromContent->setText(LANG("AutoFitColumnWidth"));
	ActionResizeFromContent->setToolTip(LANG("AutoFitColumnWidth"));
	ActionResizeFromContent->setIcon(BUFFER_ICONS("AutoFitColumnWidth"));
	connect(ActionResizeFromContent, &QAction::triggered, this, &ISListBaseForm::AutoFitColumnWidth);
	ActionsSpecial.insert(ISNamespace::AST_ResizeFromContent, ActionResizeFromContent);

	//Сброс ширины колонок
	QAction *ActionResetWidthColumn = new QAction(this);
	ActionResetWidthColumn->setText(LANG("ResetWidthColumn"));
	ActionResetWidthColumn->setToolTip(LANG("ResetWidthColumn"));
	connect(ActionResetWidthColumn, &QAction::triggered, this, &ISListBaseForm::ResetWidthColumn);
	ActionsSpecial.insert(ISNamespace::AST_ResetWidthColumn, ActionResetWidthColumn);

	//Копирование записи в буфер
	QAction *ActionCopyRecord = new QAction(this);
	ActionCopyRecord->setText(LANG("CopyRecord"));
	ActionCopyRecord->setToolTip(LANG("CopyRecord"));
	connect(ActionCopyRecord, &QAction::triggered, this, &ISListBaseForm::CopyRecord);
	ActionsSpecial.insert(ISNamespace::AST_CopyRecord, ActionCopyRecord);

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

	QAction *ActionPeriod = new QAction(ToolBar);
	ActionPeriod->setText(LANG("Period"));
	ActionPeriod->setToolTip(LANG("Period"));
	ActionPeriod->setIcon(BUFFER_ICONS("Period"));
	ActionPeriod->setMenu(new QMenu(ToolBar));
	ToolBar->addAction(ActionPeriod);

	QToolButton *ButtonPeriod = dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionPeriod));
	ButtonPeriod->setPopupMode(QToolButton::InstantPopup);
	ButtonPeriod->setCursor(CURSOR_POINTING_HAND);
	ButtonPeriod->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));

	QAction *ActionPeriodSelect = new QAction(ActionPeriod);
	ActionPeriodSelect->setText(LANG("PeriodSelect"));
	ActionPeriodSelect->setToolTip(LANG("PeriodSelect"));
	connect(ActionPeriodSelect, &QAction::triggered, this, &ISListBaseForm::Period);
	ActionPeriod->menu()->addAction(ActionPeriodSelect);

	ActionPeriod->menu()->addSeparator();

	QAction *ActionPeriodClear = new QAction(ActionPeriod);
	ActionPeriodClear->setText(LANG("PeriodClear"));
	ActionPeriodClear->setToolTip(LANG("PeriodClear"));
	connect(ActionPeriodClear, &QAction::triggered, this, &ISListBaseForm::PeriodClear);
	ActionPeriod->menu()->addAction(ActionPeriodClear);

	QAction *ActionAdditionally = new QAction(ToolBar);
	ActionAdditionally->setText(LANG("Additionally"));
	ActionAdditionally->setToolTip(LANG("Additionally"));
	ActionAdditionally->setIcon(BUFFER_ICONS("AdditionallyActions"));
	ActionAdditionally->setMenu(new QMenu(ToolBar));
	ToolBar->addAction(ActionAdditionally);

	QToolButton *ButtonAdditionally = dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionAdditionally));
	ButtonAdditionally->setPopupMode(QToolButton::InstantPopup);
	ButtonAdditionally->setCursor(CURSOR_POINTING_HAND);
	ButtonAdditionally->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));

	ActionAdditionally->menu()->addAction(GetAction(ISNamespace::AT_ShowDeleted));
	ActionAdditionally->menu()->addSeparator();
	ActionAdditionally->menu()->addAction(GetAction(ISNamespace::AT_Favorites));
	ActionAdditionally->menu()->addAction(GetAction(ISNamespace::AT_Export));
	ActionAdditionally->menu()->addAction(GetSpecialAction(ISNamespace::AST_SortDefault));
	ActionAdditionally->menu()->addAction(GetSpecialAction(ISNamespace::AST_ResizeFromContent));
	ActionAdditionally->menu()->addAction(GetSpecialAction(ISNamespace::AST_ResetWidthColumn));
	ActionAdditionally->menu()->addAction(GetSpecialAction(ISNamespace::AST_CopyRecord));

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
	if (GetAction(ISNamespace::AT_Share)) ActionObjectGroup->addAction(GetAction(ISNamespace::AT_Share));
	if (GetAction(ISNamespace::AT_AttachTask)) ActionObjectGroup->addAction(GetAction(ISNamespace::AT_AttachTask));
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
	LayoutTableView->addWidget(TableView);

	connect(TableView, &ISBaseTableView::doubleClicked, this, &ISListBaseForm::DoubleClickedTable);

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_MINIMIZEHEIGHTROWS))
	{
		TableView->verticalHeader()->setDefaultSectionSize(19);
	}

	QString SelectionBehavior = SETTING_STRING(CONST_UID_SETTING_TABLES_SELECTIONBEHAVIOR);
	if (SelectionBehavior == "SelectItems")
	{
		TableView->setSelectionBehavior(QAbstractItemView::SelectItems);
	}
	else if (SelectionBehavior == "SelectRows")
	{
		TableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	}
	else if (SelectionBehavior == "SelectColumns")
	{
		TableView->setSelectionBehavior(QAbstractItemView::SelectColumns);
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
	if (GetAction(ISNamespace::AT_Share)) ContextMenu->addAction(GetAction(ISNamespace::AT_Share));
	if (GetAction(ISNamespace::AT_AttachTask)) ContextMenu->addAction(GetAction(ISNamespace::AT_AttachTask));
	ContextMenu->addAction(GetSpecialAction(ISNamespace::AST_Note));
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateModels()
{
	if (!MetaTable->SqlModel.isEmpty()) //Если в мета-данных таблицы указана пользовательская модель, создавать её
	{
		int ObjectType = QMetaType::type((MetaTable->SqlModel + SYMBOL_STAR).toLocal8Bit().constData());
		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		SqlModel = dynamic_cast<ISSqlModelCore*>(MetaObject->newInstance(Q_ARG(PMetaClassTable *, MetaTable), Q_ARG(QObject *, TableView)));
	}
	else //Модель в мета-данных таблицы не указана, создавать стандартную (базовую)
	{
		SqlModel = new ISSqlModelCore(MetaTable, TableView);
	}

	SqlModel->FillColumns();
	SqlModel->SetShowToolTip(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWTOOLTIP));

	TableView->setModel(SqlModel);
	connect(TableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ISListBaseForm::SelectedRowEvent);//Это соединение обязательно должно быть после присваивания модели к QTableView

	QueryModel = new ISQueryModel(MetaTable, ISNamespace::QMT_List, this);
	HideSystemFields();

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
	LabelRowCount->setText(LANG("RecordsCount") + ": " + LANG("Calculated") + "...");
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

	LabelSearchResult = new QLabel(StatusBar);
	LabelSearchResult->setText(LANG("Search.Results.Empty"));
	LabelSearchResult->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LabelSearchResult->setVisible(false);
	StatusBar->addPermanentWidget(LabelSearchResult);

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
