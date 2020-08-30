#include "ISMedTech.h"
#include "ISLocalization.h"
#include "ISObjects.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_RATING = PREPARE_QUERY2("SELECT rtng_uniqueid FROM rating WHERE rtng_id = :RatingID");
//-----------------------------------------------------------------------------
ISMedTech::Object::Object() : ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
ISMedTech::Object::~Object()
{

}
//-----------------------------------------------------------------------------
void ISMedTech::Object::RegisterMetaTypes() const
{
	qRegisterMetaType<ISMedTech::RatingListForm*>("ISMedTech::RatingListForm");
	qRegisterMetaType<ISMedTech::RatingSubSystem*>("ISMedTech::RatingSubSystem");
}
//-----------------------------------------------------------------------------
void ISMedTech::Object::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISMedTech::Object::InitializePlugin() const
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISMedTech::RatingListForm::RatingListForm(QWidget *parent) : ISListBaseForm("Rating", parent)
{
	QAction *ActionFilterAll = new QAction(this);
	ActionFilterAll->setCheckable(true);
	ActionFilterAll->setChecked(true);
	ActionFilterAll->setText(LANG("Rating.Filter.All"));
	ActionFilterAll->setToolTip(LANG("Rating.Filter.All"));
	ActionFilterAll->setIcon(ISObjects::Instance().GetInterface()->GetIcon("Rating.All"));
	ActionFilterAll->setProperty("Filter", QVariant());
	connect(ActionFilterAll, &QAction::triggered, this, &ISMedTech::RatingListForm::FilterClicked);
	AddAction(ActionFilterAll, false);

	QAction *ActionFilter5 = new QAction(this);
	ActionFilter5->setCheckable(true);
	ActionFilter5->setText("5");
	ActionFilter5->setToolTip("5");
	ActionFilter5->setIcon(ISObjects::Instance().GetInterface()->GetIcon("Rating.5"));
	ActionFilter5->setProperty("Filter", 5);
	connect(ActionFilter5, &QAction::triggered, this, &ISMedTech::RatingListForm::FilterClicked);
	AddAction(ActionFilter5, false);

	QAction *ActionFilter4 = new QAction(this);
	ActionFilter4->setCheckable(true);
	ActionFilter4->setText("4");
	ActionFilter4->setToolTip("4");
	ActionFilter4->setIcon(ISObjects::Instance().GetInterface()->GetIcon("Rating.4"));
	ActionFilter4->setProperty("Filter", 4);
	connect(ActionFilter4, &QAction::triggered, this, &ISMedTech::RatingListForm::FilterClicked);
	AddAction(ActionFilter4, false);

	QAction *ActionFilter3 = new QAction(this);
	ActionFilter3->setCheckable(true);
	ActionFilter3->setText("3");
	ActionFilter3->setToolTip("3");
	ActionFilter3->setIcon(ISObjects::Instance().GetInterface()->GetIcon("Rating.3"));
	ActionFilter3->setProperty("Filter", 3);
	connect(ActionFilter3, &QAction::triggered, this, &ISMedTech::RatingListForm::FilterClicked);
	AddAction(ActionFilter3, false);

	QAction *ActionFilter2 = new QAction(this);
	ActionFilter2->setCheckable(true);
	ActionFilter2->setText("2");
	ActionFilter2->setToolTip("2");
	ActionFilter2->setIcon(ISObjects::Instance().GetInterface()->GetIcon("Rating.2"));
	ActionFilter2->setProperty("Filter", 2);
	connect(ActionFilter2, &QAction::triggered, this, &ISMedTech::RatingListForm::FilterClicked);
	AddAction(ActionFilter2, false);

	QAction *ActionFilter1 = new QAction(this);
	ActionFilter1->setCheckable(true);
	ActionFilter1->setText("1");
	ActionFilter1->setToolTip("1");
	ActionFilter1->setIcon(ISObjects::Instance().GetInterface()->GetIcon("Rating.1"));
	ActionFilter1->setProperty("Filter", 1);
	connect(ActionFilter1, &QAction::triggered, this, &ISMedTech::RatingListForm::FilterClicked);
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
ISMedTech::RatingListForm::~RatingListForm()
{

}
//-----------------------------------------------------------------------------
void ISMedTech::RatingListForm::FilterClicked()
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISMedTech::RatingSubSystem::RatingSubSystem(QWidget *parent)
	: ISInterfaceMetaForm(parent),
	CDRLoadData(false)
{
	QSplitter *Splitter = new QSplitter(Qt::Horizontal, this);
	GetMainLayout()->addWidget(Splitter);

	RatingListForm = new ISMedTech::RatingListForm(this);
	connect(RatingListForm, &ISMedTech::RatingListForm::Updated, this, &ISMedTech::RatingSubSystem::ClearCDR);
	connect(RatingListForm, &ISMedTech::RatingListForm::SelectedRowSignal, this, &ISMedTech::RatingSubSystem::SelectedRating);
	Splitter->addWidget(RatingListForm);

	//AsteriskCallsListForm = new ISAsteriskCallsListForm(this);
	//AsteriskCallsListForm->setEnabled(false);
	//AsteriskCallsListForm->GetQueryModel()->SetClassFilter("ascl_id = 0");
	//Splitter->addWidget(AsteriskCallsListForm);
}
//-----------------------------------------------------------------------------
ISMedTech::RatingSubSystem::~RatingSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISMedTech::RatingSubSystem::LoadData()
{
	RatingListForm->LoadData();
}
//-----------------------------------------------------------------------------
void ISMedTech::RatingSubSystem::SelectedRating()
{
	if (RatingListForm->GetSelectedIDs().size() == 1)
	{
		ISQuery qSelect(QS_RATING);
		qSelect.BindValue(":RatingID", RatingListForm->GetObjectID());
		if (qSelect.ExecuteFirst())
		{
			//AsteriskCallsListForm->setEnabled(true);
			//AsteriskCallsListForm->GetQueryModel()->SetClassFilter("ascl_uniqueid IS NOT NULL AND ascl_uniqueid = '" + qSelect.ReadColumn("rtng_uniqueid").toString() + '\'');
			if (CDRLoadData)
			{
				//AsteriskCallsListForm->Update();
			}
			else
			{
				//AsteriskCallsListForm->LoadData();
				CDRLoadData = true;
			}
		}
	}
	else
	{
		ClearCDR();
	}
}
//-----------------------------------------------------------------------------
void ISMedTech::RatingSubSystem::ClearCDR()
{
	//AsteriskCallsListForm->GetQueryModel()->SetClassFilter("ascl_id = 0");
	//AsteriskCallsListForm->Update();
	//AsteriskCallsListForm->setEnabled(false);
}
//-----------------------------------------------------------------------------
