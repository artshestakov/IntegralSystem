#include "StdAfx.h"
#include "ISSironaCostListForm.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISQuery.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISSironaCostListForm::ISSironaCostListForm(QWidget *parent) : ISListBaseForm("Cost", parent)
{
	LabelSum = new QLabel(this);
	LabelSum->setFont(FONT_TAHOMA_11);
	LabelSum->setText(LANG("Sirona.TotalMoney.Cost").arg(0));
	GetStatusBar()->addWidget(LabelSum);
}
//-----------------------------------------------------------------------------
ISSironaCostListForm::~ISSironaCostListForm()
{

}
//-----------------------------------------------------------------------------
void ISSironaCostListForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();

	if (GetSqlModel()->rowCount())
	{
		QVectorInt VectorInt = GetIDs();
		QString SqlIN;
		for (int ID : VectorInt)
		{
			SqlIN += QString::number(ID) + ", ";
		}
		ISSystem::RemoveLastSymbolFromString(SqlIN, 2);

		ISQuery qSelect("SELECT SUM(cost_sum) FROM cost WHERE cost_id IN(" + SqlIN + ")");
		if (qSelect.ExecuteFirst())
		{
			LabelSum->setText(LANG("Sirona.TotalMoney.Cost").arg(qSelect.ReadColumn("sum").toDouble()));
		}
	}
}
//-----------------------------------------------------------------------------
