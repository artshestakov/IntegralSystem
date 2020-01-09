#include "StdAfx.h"
#include "ISMedTechRatingListForm.h"
#include "ISLocalization.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
ISMedTechRatingListForm::ISMedTechRatingListForm(QWidget *parent) : ISListBaseForm("Rating", parent)
{
	QAction *ActionFilterAll = new QAction(this);
	ActionFilterAll->setCheckable(true);
	ActionFilterAll->setChecked(true);
	ActionFilterAll->setText(LOCALIZATION("Rating.Filter.All"));
	ActionFilterAll->setToolTip(LOCALIZATION("Rating.Filter.All"));
	ActionFilterAll->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("Rating.All"));
	ActionFilterAll->setProperty("Filter", QVariant());
	connect(ActionFilterAll, &QAction::triggered, this, &ISMedTechRatingListForm::FilterClicked);
	AddAction(ActionFilterAll, false);

	QAction *ActionFilter5 = new QAction(this);
	ActionFilter5->setCheckable(true);
	ActionFilter5->setText("5");
	ActionFilter5->setToolTip("5");
	ActionFilter5->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("Rating.5"));
	ActionFilter5->setProperty("Filter", 5);
	connect(ActionFilter5, &QAction::triggered, this, &ISMedTechRatingListForm::FilterClicked);
	AddAction(ActionFilter5, false);

	QAction *ActionFilter4 = new QAction(this);
	ActionFilter4->setCheckable(true);
	ActionFilter4->setText("4");
	ActionFilter4->setToolTip("4");
	ActionFilter4->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("Rating.4"));
	ActionFilter4->setProperty("Filter", 4);
	connect(ActionFilter4, &QAction::triggered, this, &ISMedTechRatingListForm::FilterClicked);
	AddAction(ActionFilter4, false);

	QAction *ActionFilter3 = new QAction(this);
	ActionFilter3->setCheckable(true);
	ActionFilter3->setText("3");
	ActionFilter3->setToolTip("3");
	ActionFilter3->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("Rating.3"));
	ActionFilter3->setProperty("Filter", 3);
	connect(ActionFilter3, &QAction::triggered, this, &ISMedTechRatingListForm::FilterClicked);
	AddAction(ActionFilter3, false);

	QAction *ActionFilter2 = new QAction(this);
	ActionFilter2->setCheckable(true);
	ActionFilter2->setText("2");
	ActionFilter2->setToolTip("2");
	ActionFilter2->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("Rating.2"));
	ActionFilter2->setProperty("Filter", 2);
	connect(ActionFilter2, &QAction::triggered, this, &ISMedTechRatingListForm::FilterClicked);
	AddAction(ActionFilter2, false);

	QAction *ActionFilter1 = new QAction(this);
	ActionFilter1->setCheckable(true);
	ActionFilter1->setText("1");
	ActionFilter1->setToolTip("1");
	ActionFilter1->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("Rating.1"));
	ActionFilter1->setProperty("Filter", 1);
	connect(ActionFilter1, &QAction::triggered, this, &ISMedTechRatingListForm::FilterClicked);
	AddAction(ActionFilter1, false);

	QActionGroup *ActionGroup = new QActionGroup(this);
	ActionGroup->addAction(ActionFilterAll);
	ActionGroup->addAction(ActionFilter5);
	ActionGroup->addAction(ActionFilter4);
	ActionGroup->addAction(ActionFilter3);
	ActionGroup->addAction(ActionFilter2);
	ActionGroup->addAction(ActionFilter1);
}
//-----------------------------------------------------------------------------
ISMedTechRatingListForm::~ISMedTechRatingListForm()
{

}
//-----------------------------------------------------------------------------
void ISMedTechRatingListForm::FilterClicked()
{
	QVariant SelectedFilter = sender()->property("Filter");
	if (SelectedFilter.isValid())
	{
		GetQueryModel()->SetClassFilter("rtng_rating = :Filter");
		GetQueryModel()->AddCondition(":Filter", SelectedFilter);
	}
	else
	{
		GetQueryModel()->ClearClassFilter();
		GetQueryModel()->ClearConditions();
	}

	Update();
}
//-----------------------------------------------------------------------------
