#include "ISListViewForm.h"
#include "ISBuffer.h"
#include "ISLocalizationOld.h"
#include "ISDialogsCommon.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISListViewForm::ISListViewForm(const QString &query_name, QWidget *parent)
    : ISInterfaceMetaForm(parent),
    QueryName(query_name)
{
    QToolBar *ToolBar = new QToolBar(this);
    ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ToolBar->addAction(BUFFER_ICONS("Update"), LANG("ListForm.Update"), this, &ISListViewForm::LoadData);
    GetMainLayout()->addWidget(ToolBar);

    TableView = new ISBaseTableView(this);
    GetMainLayout()->addWidget(TableView);

    ViewModel = new ISViewModel(this);
    TableView->setModel(ViewModel);
}
//-----------------------------------------------------------------------------
ISListViewForm::~ISListViewForm()
{

}
//-----------------------------------------------------------------------------
void ISListViewForm::BindValue(const QString &ParameterName, const QVariant &Value)
{
    Parameters[ParameterName] = Value;
}
//-----------------------------------------------------------------------------
void ISListViewForm::LoadData()
{
    ViewModel->Clear();

    ISTcpQuery qGetTableQuery(API_GET_TABLE_QUERY);
    qGetTableQuery.BindValue("QueryName", QueryName);
    qGetTableQuery.BindValue("Parameters", Parameters);
    if (qGetTableQuery.Execute())
    {
        QVariantMap AnswerMap = qGetTableQuery.TakeAnswer();
        QStringList FieldList = AnswerMap["FieldList"].toStringList();
        QVariantList RecordList = AnswerMap["RecordList"].toList();
        ViewModel->SetSource(FieldList, RecordList);
        TableView->resizeColumnsToContents();
        emit Updated();
    }
    else
    {
        ISMessageBox::ShowWarning(this, qGetTableQuery.GetErrorString());
    }
}
//-----------------------------------------------------------------------------
