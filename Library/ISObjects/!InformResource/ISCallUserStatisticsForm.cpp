#include "StdAfx.h"
#include "ISCallUserStatisticsForm.h"
#include "EXDefines.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISCallUserStatisticsForm::ISCallUserStatisticsForm(QWidget *parent) : ISListViewWidget(ISMetaData::GetInstanse().GetMetaQuery("CallUserStatistics"), parent)
{
	UserEdit = new ISUserEdit(this);
	connect(UserEdit, &ISUserEdit::DataChanged, this, &ISCallUserStatisticsForm::FilterChanged);
	GetToolBar()->addWidget(UserEdit);

	DateEdit = new ISDateEdit(this);
	connect(DateEdit, &ISDateEdit::DataChanged, this, &ISCallUserStatisticsForm::FilterChanged);
	GetToolBar()->addWidget(DateEdit);
}
//-----------------------------------------------------------------------------
ISCallUserStatisticsForm::~ISCallUserStatisticsForm()
{

}
//-----------------------------------------------------------------------------
void ISCallUserStatisticsForm::FilterChanged()
{
	QString FilterString;

	if (UserEdit->GetValue().isValid())
	{
		FilterString += QString("clhs_user = %1").arg(UserEdit->GetValue().toInt());
	}

	if (DateEdit->GetValue().isValid())
	{
		if (FilterString.length())
		{
			FilterString += " AND ";
		}

		FilterString += QString("clhs_creationdate::DATE = '%1'").arg(DateEdit->GetValue().toDate().toString(DATE_FORMAT_V2));
	}

	SetClassFilter(FilterString);
	Update();
}
//-----------------------------------------------------------------------------
