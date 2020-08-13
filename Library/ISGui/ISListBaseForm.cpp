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
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
#include "ISPrintingWord.h"
#endif
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
	ShowOnly(MetaTable->ShowOnly),
	IsLoadingData(false),
	SearchFlag(false)
{
	CreateActions(); //�������� ��������
	CreateSpecialActions(); //�������� ����������� ��������
	CreateToolBar(); //�������� �������
	CreateTableView(); //�������� �������
	CreateContextMenu(); //�������� ������������ ����
	CreateModels(); //�������� �������
	CreateStatusBar(); //�������� ������-����

	//�������� ����� ������� ������ ����������� ����� �������� ���� ���������
	ListIndicatorWidget = new ISListIndicatorWidget(this);
}
//-----------------------------------------------------------------------------
ISListBaseForm::~ISListBaseForm()
{
	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERCOLUMNSIZE)) //���� ����� ���������� ������ �������
	{
		for (int i = 0, c = SqlModel->columnCount(); i < c; ++i)
		{
			QString FieldName = SqlModel->headerData(i, Qt::Horizontal, Qt::UserRole).toString();
			if (!MetaTable->GetField(FieldName)->IsSystem) //���� ���� �� ��������� - ��������� ������ ����
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
PMetaTable* ISListBaseForm::GetMetaTable()
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
			ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
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
	ISLOGGER_W(QString("Not found field \"%1\" from HideField").arg(FieldName));
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
	ISLOGGER_W(QString("Not found field \"%1\" from ShowField").arg(FieldName));
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
		if (ColumnSize) //���� ���� ������ ������� � ������, ������������ ���
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
			SetSelectObjectAfterUpdate(0);
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
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Create").arg(MetaTable->LocalListName));
		return;
	}

	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->Name, 0, parentWidget());
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
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
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
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
	ObjectFormBase->SetParentObjectID(GetParentObjectID(), GetParentFilterField());
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::SetSelectObjectAfterUpdate);
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
	connect(ObjectFormBase, &ISObjectFormBase::Close, this, &ISListBaseForm::ClosingObjectForm);
	emit AddFormFromTab(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISListBaseForm::Update()
{
	IsLoadingData = true;
	SqlModel->Clear();

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
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	ISVectorInt VectorInt = GetSelectedIDs();
	if (VectorInt.size() == 1) //���� ���������� �� �������� ���� ������
	{
		if (CheckIsSystemObject())
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.NotDelete"));
			return;
		}

		if (GetCurrentRecordValue("IsDeleted").toBool()) //���� ������ ��������� - ������������
		{
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.RecoveryObjectSelected")))
			{
				if (ISGui::DeleteOrRecoveryObject(ISNamespace::DRO_Recovery, MetaTable->Name, MetaTable->Alias, VectorInt.front(), MetaTable->LocalListName)) //���� �������������� ������ �������, �������� �������
				{
					SqlModel->RemoveRecord(GetCurrentRowIndex());
					ISNotificationService::ShowNotification(ISNamespace::NotificationFormType::NFT_Recovery, MetaTable->LocalName);
				}
			}
		}
		else //���� ������ �� ��������� - �������
		{
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteSelectedRecord")))
			{
				if (ISGui::DeleteOrRecoveryObject(ISNamespace::DRO_Delete, MetaTable->Name, MetaTable->Alias, VectorInt.front(), MetaTable->LocalListName)) //���� �������� ������ �������, �������� �������
				{
					SqlModel->RemoveRecord(GetCurrentRowIndex());
					ISNotificationService::ShowNotification(ISNamespace::NotificationFormType::NFT_Delete, MetaTable->LocalName);
				}
			}
		}
	}
	else //���������� �� �������� ��������� �������
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Objects.Delete").arg(VectorInt.size())))
		{
			ISProgressForm ProgressForm(VectorInt.size(), LANG("DeletingObjects"), this);
			ProgressForm.show();
			for (int i = 0; i < VectorInt.size(); ++i)
			{
				if (ProgressForm.wasCanceled())
				{
					break;
				}
				ISGui::DeleteOrRecoveryObject(ISNamespace::DRO_Delete, MetaTable->Name, MetaTable->Alias, VectorInt[i], MetaTable->LocalListName);
				ProgressForm.IncrementValue();
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
	if (VectorInt.size() == 1) //���� ��������� ���� ������
	{
		if (CheckIsSystemObject())
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SystemObject.NotDelete"));
			return false;
		}

		if (ISMessageBox::ShowQuestion(this, LANG("Message.Object.Delete.Cascade"), LANG("Message.Object.Delete.Cascade.Help")))
		{
			int ObjectID = VectorInt.front();
			if (ISGui::DeleteCascadeObject(MetaTable->Name, MetaTable->Alias, ObjectID))
			{
				ISNotificationService::ShowNotification(LANG("NotificationForm.Title.Deleted.Cascade").arg(ObjectID));
				ISProtocol::DeleteCascadeObject(MetaTable->Name, MetaTable->LocalListName, GetObjectID());
				Update();
				return true;
			}
		}
	}
	else //��������� ��������� �������
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Objects.Delete.Cascade").arg(VectorInt.size()), LANG("Message.Object.Delete.Cascade.Help")))
		{
			ISProgressForm ProgressForm(VectorInt.size(), LANG("DeletingCascadeObjects"), this);
			ProgressForm.show();

			for (int i = 0; i < VectorInt.size(); ++i)
			{
				ProgressForm.IncrementValue();

				int ObjectID = VectorInt[i];
				if (ISGui::DeleteCascadeObject(MetaTable->Name, MetaTable->Alias, ObjectID))
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
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_IS_DELETED))
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

	ISProgressForm ProgressForm(SqlModel->rowCount(), LANG("Export.Process.Prepare"), this);
	connect(&ProgressForm, &ISProgressForm::canceled, ExportWorker, &ISExportWorker::Cancel);
	connect(ExportWorker, &ISExportWorker::ExportedRow, &ProgressForm, static_cast<void(ISProgressForm::*)(void)>(&ISProgressForm::IncrementValue));
	//connect(ExportWorker, &ISExportWorker::Message, &ProgressForm, &ISProgressForm::SetText);
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

	//������� ��������
	QAction *ActionDeleteCascade = ISControls::CreateActionDeleteCascade(this);
	connect(ActionDeleteCascade, &QAction::triggered, this, &ISListBaseForm::DeleteCascade);
	Actions[ISNamespace::AT_DeleteCascade] = ActionDeleteCascade;

	//��������
	QAction *ActionUpdate = ISControls::CreateActionUpdate(this);
	connect(ActionUpdate, &QAction::triggered, this, &ISListBaseForm::Update);
	Actions[ISNamespace::AT_Update] = ActionUpdate;

	//���������� ���������� ������
	QAction *ActionShowActual = new QAction(BUFFER_ICONS("ShowActual"), LANG("ListForm.ShowActual"), this);
	ActionShowActual->setToolTip(LANG("ListForm.ShowActual.ToolTip"));
	ActionShowActual->setCheckable(true);
	ActionShowActual->setChecked(true);
	ActionShowActual->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F1));
	connect(ActionShowActual, &QAction::triggered, this, &ISListBaseForm::ShowActual);
	Actions[ISNamespace::AT_ShowActual] = ActionShowActual;

	//���������� ��������� ������
	QAction *ActionShowDeleted = new QAction(BUFFER_ICONS("ShowDeleted"), LANG("ListForm.ShowDeleted"), this);
	ActionShowDeleted->setToolTip(LANG("ListForm.ShowDeleted.ToolTip"));
	ActionShowDeleted->setCheckable(true);
	ActionShowDeleted->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F2));
	connect(ActionShowDeleted, &QAction::triggered, this, &ISListBaseForm::ShowDeleted);
	Actions[ISNamespace::AT_ShowDeleted] = ActionShowDeleted;

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
	QAction *ActionExport = ISControls::CreateActionExport(this);
	connect(ActionExport, &QAction::triggered, this, &ISListBaseForm::Export);
	Actions[ISNamespace::AT_Export] = ActionExport;

	//������
	QAction *ActionPrint = ISControls::CreateActionPrint(this);
	connect(ActionPrint, &QAction::triggered, this, &ISListBaseForm::Print);
	Actions[ISNamespace::AT_Print] = ActionPrint;

	//���������
	QAction *ActionFavorites = new QAction(BUFFER_ICONS("Favorites"), LANG("Favorites"), this);
	connect(ActionFavorites, &QAction::triggered, this, &ISListBaseForm::ShowFavorites);
	Actions[ISNamespace::AT_Favorites] = ActionFavorites;

	//��������� ����������
	QAction *ActionSystemInformation = ISControls::CreateActionRecordInformartion(this);
	connect(ActionSystemInformation, &QAction::triggered, this, &ISListBaseForm::ShowSystemInfo);
	Actions[ISNamespace::AT_SystemInfo] = ActionSystemInformation;

	//������ ������
	QAction *ActionNavigationBegin = ISControls::CreateActionNavigationBegin(this);
	connect(ActionNavigationBegin, &QAction::triggered, this, &ISListBaseForm::NavigationSelectBeginRecord);
	Actions[ISNamespace::AT_NavigationBegin] = ActionNavigationBegin;

	//���������� ������
	QAction *ActionNavigationPrevious = ISControls::CreateActionNavigationPrevious(this);
	connect(ActionNavigationPrevious, &QAction::triggered, this, &ISListBaseForm::NavigationSelectPreviousRecord);
	Actions[ISNamespace::AT_NavigationPrevious] = ActionNavigationPrevious;

	//��������� ������
	QAction *ActionNavigationNext = ISControls::CreateActionNavigationNext(this);
	connect(ActionNavigationNext, &QAction::triggered, this, &ISListBaseForm::NavigationSelectNextRecord);
	Actions[ISNamespace::AT_NavigationNext] = ActionNavigationNext;

	//��������� ������
	QAction *ActionNavigationLast = ISControls::CreateActionNavigationLast(this);
	connect(ActionNavigationLast, &QAction::triggered, this, &ISListBaseForm::NavigationSelectLastRecord);
	Actions[ISNamespace::AT_NavigationLast] = ActionNavigationLast;
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateSpecialActions()
{
	//���������� �� ���������
	QAction *ActionSortDefault = ISControls::CreateActionSortDefault(this);
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
	ContextMenu->addAction(GetSpecialAction(ISNamespace::AST_Note));
}
//-----------------------------------------------------------------------------
void ISListBaseForm::CreateModels()
{
	if (!MetaTable->SqlModel.isEmpty()) //���� � ����-������ ������� ������� ���������������� ������, ��������� �
	{
		SqlModel = ISAlgorithm::CreatePointer<ISSqlModelCore *>(MetaTable->SqlModel, Q_ARG(PMetaTable *, MetaTable), Q_ARG(QObject *, TableView));
	}
	else //������ � ����-������ ������� �� �������, ��������� ����������� (�������)
	{
		SqlModel = new ISSqlModelCore(MetaTable, TableView);
	}

	SqlModel->FillColumns();
	SqlModel->SetShowToolTip(SETTING_BOOL(CONST_UID_SETTING_TABLES_SHOWTOOLTIP));

	QueryModel = new ISQueryModel(MetaTable, ISNamespace::QMT_List, this);

	ISSortingMetaTable *MetaSorting = ISSortingBuffer::Instance().GetSorting(MetaTable->Name);
	if (MetaSorting) //���� ���������� ��� ���� ������� ��� ����������, ������������ �
	{
		SqlModel->SetSorting(MetaSorting->FieldName, MetaSorting->Order);
		QueryModel->SetOrderField(MetaTable->Alias + '_' + MetaSorting->FieldName.toLower(), MetaSorting->FieldName, MetaSorting->Order);
	}

	TableView->setModel(SqlModel);
	connect(TableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ISListBaseForm::SelectedRowEvent);//��� ���������� ����������� ������ ���� ����� ������������ ������ � QTableView

	HideSystemFields();

	ModelThreadQuery = new ISModelThreadQuery(this);
	connect(ModelThreadQuery, &ISModelThreadQuery::Started, this, &ISListBaseForm::ModelThreadStarted);
	connect(ModelThreadQuery, &ISModelThreadQuery::Finished, this, &ISListBaseForm::ModelThreadFinished);
	connect(ModelThreadQuery, &ISModelThreadQuery::ExecutedQuery, this, &ISListBaseForm::ModelThreadLoadingData);
	connect(ModelThreadQuery, &ISModelThreadQuery::Results, SqlModel, &ISSqlModelCore::SetRecords);
	connect(ModelThreadQuery, &ISModelThreadQuery::ErrorConnection, this, &ISListBaseForm::ModelThreadErrorConnection);
	connect(ModelThreadQuery, &ISModelThreadQuery::ErrorQuery, this, &ISListBaseForm::ModelThreadErrorQuery);
	ModelThreadQuery->start(QThread::TimeCriticalPriority); //������ ������
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
