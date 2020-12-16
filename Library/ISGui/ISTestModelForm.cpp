#include "ISTestModelForm.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
#define TABLE_NAME "_Users"
//-----------------------------------------------------------------------------
ISTestModelForm::ISTestModelForm(QWidget *parent)
	: ISInterfaceMetaForm(parent),
	TcpModel(new ISTcpModel(this)),
	qGetTableData(new ISTcpQuery(API_GET_TABLE_DATA))
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
	if (qGetTableData->Execute())
	{
		SetSource(qGetTableData->TakeAnswer());
	}
}
//-----------------------------------------------------------------------------
void ISTestModelForm::SetSource(const QVariantMap &TcpAnswer)
{
	TcpModel->SetSource(TcpAnswer["FieldList"].toList(), TcpAnswer["RecordList"].toList());

	QVariantMap ServiceInfo = TcpAnswer["ServiceInfo"].toMap();
	QString SortingField = ServiceInfo["SortingField"].toString();
	Qt::SortOrder SortingOrder = static_cast<Qt::SortOrder>(ServiceInfo["SortingOrder"].toUInt());
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
	qGetTableData->BindValue("TableName", TABLE_NAME);
	qGetTableData->BindValue("SortingField", TcpModel->headerData(LogicalIndex, Qt::Horizontal, Qt::UserRole));
	qGetTableData->BindValue("SortingOrder", Order);
	if (qGetTableData->Execute())
	{
		SetSource(qGetTableData->TakeAnswer());
	}
}
//-----------------------------------------------------------------------------
