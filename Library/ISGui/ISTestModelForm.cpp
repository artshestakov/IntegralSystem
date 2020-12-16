#include "ISTestModelForm.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
#define TABLE_NAME "_Users"
//-----------------------------------------------------------------------------
ISTestModelForm::ISTestModelForm(QWidget *parent)
	: ISInterfaceMetaForm(parent),
	TcpModel(new ISTcpModel(this)),
	qGetTableData(new ISTcpQuery(API_GET_TABLE_DATA)),
	SortingField("ID"),
	SortingOrder(Qt::AscendingOrder)
{
	TableView = new ISBaseTableView(this);
	GetMainLayout()->addWidget(TableView);
}
//-----------------------------------------------------------------------------
ISTestModelForm::~ISTestModelForm()
{

}
//-----------------------------------------------------------------------------
void ISTestModelForm::LoadData()
{
	qGetTableData->BindValue("TableName", TABLE_NAME);
	qGetTableData->BindValue("SortingField", SortingField);
	qGetTableData->BindValue("SortingOrder", SortingOrder);
	if (qGetTableData->Execute())
	{
		SetSource(qGetTableData->TakeAnswer());
	}
}
//-----------------------------------------------------------------------------
void ISTestModelForm::SetSource(const QVariantMap &TcpAnswer)
{
	TcpModel->SetSource(TcpAnswer["FieldList"].toList(), TcpAnswer["RecordList"].toList());
	TcpModel->SetSorting(SortingField, SortingOrder);
	if (!TableView->model())
	{
		TableView->setModel(TcpModel);
	}
	disconnect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISTestModelForm::SortingChanged);
	TableView->horizontalHeader()->setSortIndicator(TcpModel->GetFieldIndex(SortingField), SortingOrder);
	connect(TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ISTestModelForm::SortingChanged);
}
//-----------------------------------------------------------------------------
void ISTestModelForm::SortingChanged(int LogicalIndex, Qt::SortOrder Order)
{
	SortingField = TcpModel->headerData(LogicalIndex, Qt::Horizontal, Qt::UserRole).toString();
	SortingOrder = Order;

	qGetTableData->BindValue("TableName", TABLE_NAME);
	qGetTableData->BindValue("SortingField", SortingField);
	qGetTableData->BindValue("SortingOrder", SortingOrder);
	if (qGetTableData->Execute())
	{
		SetSource(qGetTableData->TakeAnswer());
	}
}
//-----------------------------------------------------------------------------
