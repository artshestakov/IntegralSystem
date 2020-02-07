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
ISFullTextSearchForm::ISFullTextSearchForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	Stopped = false;
	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	LineEdit = new ISLineEdit(this);
	LineEdit->SetPlaceholderText(LANG("InputThisSearchQuery"));
	LineEdit->SetIcon(BUFFER_ICONS("Search"));
	LineEdit->SetFont(DEFINES_GUI.FONT_TAHOMA_10);
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
	GetMainLayout()->addWidget(ProgressBar);

	QHBoxLayout *LayoutLabels = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutLabels);

	LabelSearch = new QLabel(this);
	LabelSearch->setVisible(false);
	LayoutLabels->addWidget(LabelSearch);

	LayoutLabels->addStretch();

	LabelResult = new QLabel(this);
	LayoutLabels->addWidget(LabelResult);

	Frame = ISControls::CreateHorizontalLine(this);
	Frame->setVisible(false);
	GetMainLayout()->addWidget(Frame);

	ScrollArea = new ISScrollArea(this);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	GetMainLayout()->addWidget(ScrollArea);

	Database = QSqlDatabase::cloneDatabase(ISDatabase::GetInstance().GetDefaultDB(), "123");
	EventLoop = new QEventLoop(this);

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
void ISFullTextSearchForm::BeforeSearch()
{
	ISGui::SetWaitGlobalCursor(true);

	while (WidgetList.count())
	{
		QWidget *Widget = WidgetList.takeFirst();
		if (Widget)
		{
			delete Widget;
		}
	}

	LineEdit->setEnabled(false);
	ButtonSearch->setEnabled(false);
	ButtonStop->setEnabled(true);
	ProgressBar->setValue(0);
	LabelSearch->setVisible(true);
	LabelResult->clear();
}
//-----------------------------------------------------------------------------
void ISFullTextSearchForm::Search()
{
	BeforeSearch();
	
	QList<PMetaClassTable*> Tables = ISMetaData::GetInstanse().GetTables();
	ProgressBar->setMaximum(Tables.count());

	for (PMetaClassTable *MetaTable : Tables) //����� ������
	{
		if (Stopped)
		{
			ISGui::SetWaitGlobalCursor(false);
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.StopFullTextSearch")))
			{
				ProgressBar->setValue(0);
				AfterSearch();
				LineEdit->SetFocus();
				LineEdit->SelectAll();
				return;
			}
			else
			{
				ISGui::SetWaitGlobalCursor(true);
			}
			Stopped = false;
		}

		ProgressBar->setValue(ProgressBar->value() + 1);
		LabelSearch->setText(LANG("SearchIn") + ": " + MetaTable->GetLocalListName());

		if (!MetaTable->GetIsSystem()) //������� �� ���������
		{
			QFuture<void> Future = QtConcurrent::run(this, &ISFullTextSearchForm::Execute, CreateQuery(MetaTable), LineEdit->GetValue());
			FutureWatcher->setFuture(Future);
			EventLoop->exec();
		}
	}

	LabelSearch->setText(LANG("BuildingListResult"));
	ISGui::RepaintWidget(LabelSearch);
	int ResultCount = 0;

	for (const auto &MapItem : Map.toStdMap()) //����� ������
	{
		QString TableName = MapItem.first;
		QVectorInt VectorInt = MapItem.second;
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);

		for (int ObjectID : VectorInt) //����� ��������
		{
			ResultCount++;

			ISLabelLink *LabelLink = new ISLabelLink(ScrollArea);
			LabelLink->setText(QString::number(ResultCount) + ". " + MetaTable->GetLocalName() + ": " + ISCore::GetObjectName(MetaTable, ObjectID));
			LabelLink->setSizePolicy(QSizePolicy::Maximum, LabelLink->sizePolicy().verticalPolicy());
			LabelLink->setProperty("TableName", TableName);
			LabelLink->setProperty("ObjectID", ObjectID);
			connect(LabelLink, &ISLabelLink::Clicked, this, &ISFullTextSearchForm::ClickedRecord);
			dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addWidget(LabelLink/*, 0, Qt::AlignTop*/);
			WidgetList.append(LabelLink);
		}
	}
	dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addStretch();
	LabelResult->setText(LANG("Search.ResultCount") + ": " + QString::number(ResultCount));
	AfterSearch();
}
//-----------------------------------------------------------------------------
void ISFullTextSearchForm::AfterSearch()
{
	LineEdit->setEnabled(true);
	ButtonSearch->setEnabled(true);
	ButtonStop->setEnabled(false);
	LabelSearch->setVisible(false);
	Frame->setVisible(true);
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISFullTextSearchForm::Execute(const QString &QueryText, const QVariant &QueryValue)
{
	if (Database.open())
	{
		{
			ISQuery qSelect(Database, QueryText);
			qSelect.BindValue(":Value", QueryValue);
			if (qSelect.Execute())
			{
				while (qSelect.Next())
				{
					int ObjectID = qSelect.ReadColumn("ID").toInt();
					QString TableName = qSelect.ReadColumn("table_name").toString();

					if (Map.contains(TableName))
					{
						Map[TableName].append(ObjectID);
					}
					else
					{
						QVectorInt VectorInt;
						VectorInt.append(ObjectID);
						Map.insert(TableName, VectorInt);
					}
				}
			}
		}
		Database.close();
	}
}
//-----------------------------------------------------------------------------
QString ISFullTextSearchForm::CreateQuery(PMetaClassTable *MetaTable) const
{
	QString QueryText = "WITH query AS \n(\n";
	QueryText += "SELECT " + MetaTable->GetAlias() + "_id AS ID, concat(";
	for (PMetaClassField *MetaField : MetaTable->GetFields())
	{
		if (!ISMetaData::GetInstanse().GetAssociationTypes().GetSearch(MetaField->GetType())) //���� ���������� �� ����� �� ���� ���� ����
		{
			continue;
		}

		if (MetaField->GetQueryText().length()) //���� ���� �������� ����-��������
		{
			continue;
		}

		PMetaClassForeign *MetaForeign = MetaField->GetForeign();
		if (MetaForeign)
		{
			PMetaClassTable *MetaForeignTable = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->GetForeignClass());
			QueryText += "(SELECT concat(";
			for (const QString &FieldName : MetaForeign->GetForeignViewNameField().split(';'))
			{
				QueryText += MetaForeignTable->GetAlias() + '_' + FieldName + ", ";
			}
			ISSystem::RemoveLastSymbolFromString(QueryText, 2);
			QueryText += ") FROM " + MetaForeignTable->GetName() + SYMBOL_SPACE;
			QueryText += "WHERE " + MetaForeignTable->GetAlias() + '_' + MetaForeign->GetForeginField() + " = " + MetaTable->GetAlias() + '_' + MetaField->GetName() + ')';
		}
		else
		{
			QueryText += MetaTable->GetAlias() + '_' + MetaField->GetName();
		}
		QueryText += ", ";
	}
	
	ISSystem::RemoveLastSymbolFromString(QueryText, 2);
	QueryText += ") \nFROM " + MetaTable->GetName() + " \n";
	QueryText += "ORDER BY " + MetaTable->GetAlias() + "_id \n)\n";
	QueryText += "SELECT id, '" + MetaTable->GetName() + "' AS table_name \n";
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
	Stopped = true;
}
//-----------------------------------------------------------------------------
