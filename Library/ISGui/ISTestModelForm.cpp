#include "ISTestModelForm.h"
#include "ISTcpQuery.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTestModelForm::ISTestModelForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	TableView = new ISBaseTableView(this);
	GetMainLayout()->addWidget(TableView);

	TcpModel = new ISTcpModel(this);
}
//-----------------------------------------------------------------------------
ISTestModelForm::~ISTestModelForm()
{

}
//-----------------------------------------------------------------------------
void ISTestModelForm::LoadData()
{
	ISTcpQuery qGetTableData(API_GET_TABLE_DATA);
	qGetTableData.BindValue("TableName", "_Test");
	if (qGetTableData.Execute())
	{
		QVariantMap Answer = qGetTableData.GetAnswer();
		TcpModel->SetData(Answer["FieldList"].toList(), Answer["RecordList"].toList());
		TableView->setModel(TcpModel);
	}
}
//-----------------------------------------------------------------------------
