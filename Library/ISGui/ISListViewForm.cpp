#include "ISListViewForm.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISDatabase.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISListViewForm::ISListViewForm(const QString &query_name, QWidget *parent)
	: ISInterfaceMetaForm(parent),
	QueryName(query_name),
	SqlText(BUFFER_SQL(QueryName))
{
	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ToolBar->addAction(BUFFER_ICONS("Update"), LANG("ListForm.Update"), this, &ISListViewForm::LoadData);
	GetMainLayout()->addWidget(ToolBar);

	TableView = new ISBaseTableView(this);
	GetMainLayout()->addWidget(TableView);

	SqlModel = new QSqlQueryModel(this);
	TableView->setModel(SqlModel);
}
//-----------------------------------------------------------------------------
ISListViewForm::~ISListViewForm()
{

}
//-----------------------------------------------------------------------------
void ISListViewForm::BindValue(const QString &ParameterName, const QVariant &Value)
{
	Paramters.emplace(ParameterName, Value);
}
//-----------------------------------------------------------------------------
void ISListViewForm::LoadData()
{
	QSqlQuery SqlQuery(ISDatabase::Instance().GetDB(CONNECTION_DEFAULT));
	if (SqlQuery.prepare(SqlText))
	{
		for (const auto &MapItem : Paramters)
		{
			SqlQuery.bindValue(MapItem.first, MapItem.second);
		}
		
		if (SqlQuery.exec())
		{
			SqlModel->setQuery(SqlQuery);
			TableView->resizeColumnsToContents();
			emit Updated();
		}
		else
		{
			ISMessageBox::ShowWarning(this, SqlQuery.lastError().text(), SqlText);
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, SqlQuery.lastError().text(), SqlText);
	}
}
//-----------------------------------------------------------------------------
QSqlQueryModel* ISListViewForm::GetSqlModel()
{
	return SqlModel;
}
//-----------------------------------------------------------------------------
