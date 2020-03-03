#include "ISMedTechRatingSubSystem.h"
#include "ISQuery.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_RATING = PREPARE_QUERY2("SELECT rtng_uniqueid FROM rating WHERE rtng_id = :RatingID");
//-----------------------------------------------------------------------------
ISMedTechRatingSubSystem::ISMedTechRatingSubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	CDRLoadData = false;

	QSplitter *Splitter = new QSplitter(Qt::Horizontal, this);
	GetMainLayout()->addWidget(Splitter);

	RatingListForm = new ISMedTechRatingListForm(this);
	connect(RatingListForm, &ISMedTechRatingListForm::Updated, this, &ISMedTechRatingSubSystem::ClearCDR);
	connect(RatingListForm, &ISMedTechRatingListForm::SelectedRowSignal, this, &ISMedTechRatingSubSystem::SelectedRating);
	Splitter->addWidget(RatingListForm);

	AsteriskCallsListForm = new ISAsteriskCallsListForm(this);
	AsteriskCallsListForm->setEnabled(false);
	AsteriskCallsListForm->GetQueryModel()->SetClassFilter("ascl_id = 0");
	Splitter->addWidget(AsteriskCallsListForm);
}
//-----------------------------------------------------------------------------
ISMedTechRatingSubSystem::~ISMedTechRatingSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISMedTechRatingSubSystem::LoadData()
{
	RatingListForm->LoadData();
}
//-----------------------------------------------------------------------------
void ISMedTechRatingSubSystem::SelectedRating()
{
	if (RatingListForm->GetSelectedIDs().count() == 1)
	{
		ISQuery qSelect(QS_RATING);
		qSelect.BindValue(":RatingID", RatingListForm->GetObjectID());
		if (qSelect.ExecuteFirst())
		{
			QString UniqueID = qSelect.ReadColumn("rtng_uniqueid").toString();

			AsteriskCallsListForm->setEnabled(true);
			AsteriskCallsListForm->GetQueryModel()->SetClassFilter("ascl_uniqueid IS NOT NULL AND ascl_uniqueid = '" + UniqueID + '\'');
			if (CDRLoadData)
			{
				AsteriskCallsListForm->Update();
			}
			else
			{
				AsteriskCallsListForm->LoadData();
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
void ISMedTechRatingSubSystem::ClearCDR()
{
	AsteriskCallsListForm->GetQueryModel()->SetClassFilter("ascl_id = 0");
	AsteriskCallsListForm->Update();
	AsteriskCallsListForm->setEnabled(false);
}
//-----------------------------------------------------------------------------
