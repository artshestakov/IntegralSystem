#include "StdAfx.h"
#include "ISSironaResearchListForm.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISQuery.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static QString QS_RESEARCH = PREPARE_QUERY("SELECT pmtp_uid, resr_money "
										   "FROM research "
										   "LEFT JOIN paymenttype ON pmtp_id = resr_payment "
										   "WHERE resr_id IN(:IN)");
//-----------------------------------------------------------------------------
ISSironaResearchListForm::ISSironaResearchListForm(QWidget *parent) : ISListBaseForm("Research", parent)
{
	LabelSummary = new QLabel(this);
	LabelSummary->setFont(FONT_TAHOMA_11);
	LabelSummary->setText(LOCALIZATION("Sirona.TotalMoney.Research").arg(0).arg(0).arg(0));
	GetStatusBar()->addWidget(LabelSummary);
}
//-----------------------------------------------------------------------------
ISSironaResearchListForm::~ISSironaResearchListForm()
{

}
//-----------------------------------------------------------------------------
void ISSironaResearchListForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();
	double Cash = 0;
	double NonCash = 0;

	if (GetSqlModel()->rowCount())
	{
		QVectorInt VectorInt = GetIDs();
		QString SqlIN;
		for (int ID : VectorInt)
		{
			SqlIN += QString::number(ID) + ", ";
		}
		ISSystem::RemoveLastSymbolFromString(SqlIN, 2);

		QString SqlText = QS_RESEARCH;
		ISQuery qSelect(SqlText.replace(":IN", SqlIN));
		if (qSelect.Execute())
		{
			while (qSelect.Next())
			{
				ISUuid PaymentUID = qSelect.ReadColumn("pmtp_uid");
				double Money = qSelect.ReadColumn("resr_money").toDouble();

				if (PaymentUID == "{1D7CED21-735E-4DF9-99B9-3CA30990AFC9}") //Наличные
				{
					Cash += Money;
				}
				else if (PaymentUID == "{6C5C84DD-0BF6-4AD7-8D62-441D8CD90AEF}") //Безналичные
				{
					NonCash += Money;
				}
			}
		}
	}

	LabelSummary->setText(LOCALIZATION("Sirona.TotalMoney.Research").arg(Cash).arg(NonCash).arg(Cash + NonCash));
}
//-----------------------------------------------------------------------------
