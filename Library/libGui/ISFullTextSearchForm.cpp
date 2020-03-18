#include "ISFullTextSearchForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDatabase.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISLabels.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISFullTextSearchForm::ISFullTextSearchForm(QWidget *parent)
	: ISInterfaceMetaForm(parent),
	Stopped(false),
	EventLoop(new QEventLoop(this))
{
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	LineEdit = new ISLineEdit(this);
	LineEdit->SetPlaceholderText(LANG("InputThisSearchQuery"));
	LineEdit->SetIcon(BUFFER_ICONS("Search"));
	LineEdit->SetFont(ISDefines::Gui::FONT_TAHOMA_10);
	Layout->addWidget(LineEdit);

	ButtonSearch = new ISPushButton(this);
	ButtonSearch->setText(LANG("Search"));
	ButtonSearch->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonSearch, &ISPushButton::clicked, this, &ISFullTextSearchForm::Search);
	Layout->addWidget(ButtonSearch);

	ButtonStop = new ISPushButton(this);
	ButtonStop->setText(LANG("Search.Stop"));
	ButtonStop->setEnabled(false);
	connect(ButtonStop, &ISPushButton::clicked, this, &ISFullTextSearchForm::Stop);
	Layout->addWidget(ButtonStop);

	ProgressBar = new QProgressBar(this);
	ProgressBar->setVisible(false);
	connect(this, &ISFullTextSearchForm::SetProgressMaximum, ProgressBar, &QProgressBar::setMaximum);
	connect(this, &ISFullTextSearchForm::SetProgressValue, ProgressBar, &QProgressBar::setValue);
	GetMainLayout()->addWidget(ProgressBar);

	QHBoxLayout *LayoutLabels = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutLabels);

	LabelSearch = new QLabel(this);
	LabelSearch->setVisible(false);
	connect(this, &ISFullTextSearchForm::SetLabelSearch, LabelSearch, &QLabel::setText);
	LayoutLabels->addWidget(LabelSearch);

	LayoutLabels->addStretch();

	LabelResult = new QLabel(this);
	LayoutLabels->addWidget(LabelResult);

	Frame = ISControls::CreateHorizontalLine(this);
	Frame->setVisible(false);
	GetMainLayout()->addWidget(Frame);

	ScrollArea = new ISScrollArea(this);
	GetMainLayout()->addWidget(ScrollArea);

	FutureWatcher = new QFutureWatcher<void>(this);
	connect(FutureWatcher, &QFutureWatcher<void>::finished, EventLoop, &QEventLoop::quit);
}
//-----------------------------------------------------------------------------
ISFullTextSearchForm::~ISFullTextSearchForm()
{

}
//-----------------------------------------------------------------------------
void ISFullTextSearchForm::LoadData()
{
	LineEdit->SetFocus();
}
//-----------------------------------------------------------------------------
void ISFullTextSearchForm::SetSearchInProgress(bool InProgress)
{
	ISGui::SetWaitGlobalCursor(InProgress);
	LineEdit->setEnabled(!InProgress);
	ButtonSearch->setEnabled(!InProgress);
	ButtonStop->setEnabled(InProgress);
	ProgressBar->setVisible(InProgress);
	LabelSearch->setVisible(InProgress);

	if (InProgress)
	{
		while (!WidgetList.isEmpty())
		{
			delete WidgetList.takeFirst();
		}
		ProgressBar->setValue(0);
		LabelResult->clear();
		MapResult.clear();
	}
	else
	{
		LineEdit->SetFocus();
		LineEdit->SelectAll();
		ProgressBar->setValue(0);
		Frame->setVisible(true);
	}
}
//-----------------------------------------------------------------------------
void ISFullTextSearchForm::Search()
{
	if (LineEdit->GetValue().toString().isEmpty())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.SearchFieldNotValid"));
		LineEdit->BlinkRed();
		return;
	}

	SetSearchInProgress(true);
	
	QFuture<void> Future = QtConcurrent::run(this, &ISFullTextSearchForm::Execute, LineEdit->GetValue());
	FutureWatcher->setFuture(Future);
	EventLoop->exec();

	LabelSearch->setText(LANG("BuildingListResult"));
	ISGui::RepaintWidget(LabelSearch);
	int ResultCount = 0;

	if (ScrollArea->widget()->layout())
	{
		delete ScrollArea->widget()->layout();
	}

	ScrollArea->widget()->setLayout(new QVBoxLayout());
	for (const auto &MapItem : MapResult) //Обход таблиц
	{
		QString TableName = MapItem.first;
		ISVectorInt VectorInt = MapItem.second;
		PMetaTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);

		for (int ObjectID : VectorInt) //Обход объектов
		{
			ISLabelLink *LabelLink = new ISLabelLink(ScrollArea);
			LabelLink->setText(QString::number(++ResultCount) + ". " + MetaTable->LocalName + ": " + ISCore::GetObjectName(MetaTable, ObjectID));
			LabelLink->setSizePolicy(QSizePolicy::Maximum, LabelLink->sizePolicy().verticalPolicy());
			LabelLink->setProperty("TableName", TableName);
			LabelLink->setProperty("ObjectID", ObjectID);
			connect(LabelLink, &ISLabelLink::Clicked, this, &ISFullTextSearchForm::ClickedRecord);
			dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addWidget(LabelLink);
			WidgetList.append(LabelLink);
		}
	}

	if (!ResultCount) //Если поиск не дал результатов
	{
		QLabel *LabelNotFound = new QLabel(LANG("FullSearchTextNotResult"), ScrollArea);
		ISGui::SetFontWidgetBold(LabelNotFound, true);
		dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addWidget(LabelNotFound);
		WidgetList.append(LabelNotFound);
	}

	dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addStretch();
	LabelResult->setText(LANG("Search.ResultCount") + ": " + QString::number(ResultCount));
	SetSearchInProgress(false);
}
//-----------------------------------------------------------------------------
void ISFullTextSearchForm::Execute(const QVariant &SearchValue)
{
	ISConnectOptionDB ConnectOption = ISDatabase::Instance().GetOption(CONNECTION_DEFAULT);
	bool Result = ISDatabase::Instance().Connect(CONNECTION_FULL_TEXT_SEARCH, ConnectOption.Host, ConnectOption.Port, ConnectOption.Name, ConnectOption.Login, ConnectOption.Password);
	if (Result)
	{
		std::vector<PMetaTable*> Tables = ISMetaData::GetInstanse().GetTables();
		emit SetProgressMaximum(Tables.size());

		int ProgressValue = 0;
		for (PMetaTable *MetaTable : Tables) //Обход не системных таблиц
		{
			if (Stopped)
			{
				break;
				Stopped = false;
			}

			emit SetProgressValue(++ProgressValue);
			emit SetLabelSearch(LANG("SearchIn") + ": " + MetaTable->LocalListName);

			if (MetaTable->IsSystem)
			{
				continue;
			}

			ISQuery qSelect(ISDatabase::Instance().GetDB(CONNECTION_FULL_TEXT_SEARCH), CreateQuery(MetaTable));
			qSelect.BindValue(":Value", SearchValue);
			Result = qSelect.Execute();
			if (Result)
			{
				while (qSelect.Next())
				{
					int ObjectID = qSelect.ReadColumn("ID").toInt();
					QString TableName = qSelect.ReadColumn("table_name").toString();
					MapResult.count(TableName) ? MapResult[TableName].emplace_back(ObjectID) : MapResult.emplace(TableName, ISVectorInt{ ObjectID });
				}
			}
		}
		ISDatabase::Instance().Disconnect(CONNECTION_FULL_TEXT_SEARCH);
	}
}
//-----------------------------------------------------------------------------
QString ISFullTextSearchForm::CreateQuery(PMetaTable *MetaTable) const
{
	QString QueryText = "WITH query AS \n(\n";
	QueryText += "SELECT " + MetaTable->Alias + "_id AS ID, concat(";
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		if (!ISMetaData::GetInstanse().GetSearch(MetaField->Type)) //Если разрешение по поиск по типу поля есть
		{
			continue;
		}

		if (!MetaField->QueryText.isEmpty()) //Если поле является мета-запросом
		{
			continue;
		}

		if (MetaField->Foreign)
		{
			PMetaTable *MetaForeignTable = ISMetaData::GetInstanse().GetMetaTable(MetaField->Foreign->ForeignClass);
			QueryText += "(SELECT concat(";
			for (const QString &FieldName : MetaField->Foreign->ForeignViewNameField.split(';'))
			{
				QueryText += MetaForeignTable->Alias + '_' + FieldName + ", ";
			}
			ISSystem::RemoveLastSymbolFromString(QueryText, 2);
			QueryText += ") FROM " + MetaForeignTable->Name + SYMBOL_SPACE;
			QueryText += "WHERE " + MetaForeignTable->Alias + '_' + MetaField->Foreign->ForeignField + " = " + MetaTable->Alias + '_' + MetaField->Name + ')';
		}
		else
		{
			QueryText += MetaTable->Alias + '_' + MetaField->Name;
		}
		QueryText += ", ";
	}
	
	ISSystem::RemoveLastSymbolFromString(QueryText, 2);
	QueryText += ") \nFROM " + MetaTable->Name + " \n";
	QueryText += "ORDER BY " + MetaTable->Alias + "_id \n)\n";
	QueryText += "SELECT id, '" + MetaTable->Name + "' AS table_name \n";
	QueryText += "FROM query \n";
	QueryText += "WHERE lower(concat) LIKE '%' || :Value || '%'";
	return QueryText;
}
//-----------------------------------------------------------------------------
void ISFullTextSearchForm::ClickedRecord()
{
	ISGui::CreateObjectForm(ISNamespace::OFT_Edit, sender()->property("TableName").toString(), sender()->property("ObjectID").toInt())->show();
}
//-----------------------------------------------------------------------------
void ISFullTextSearchForm::Stop()
{
	Mutex.lock();
	Stopped = true;
	Mutex.unlock();
}
//-----------------------------------------------------------------------------
