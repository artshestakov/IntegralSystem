#include "StdAfx.h"
#include "ISScoreObjectForm.h"
//-----------------------------------------------------------------------------
ISScoreObjectForm::ISScoreObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	EditCompetitor = GetFieldWidget("Competitor");
	connect(EditCompetitor, &ISFieldEditBase::DataChanged, this, &ISScoreObjectForm::CompetitorChanged);

	EditAuditors = GetFieldWidget("Auditors");
	EditJournals = GetFieldWidget("Journals");
	EditSeminars = GetFieldWidget("Seminars");
	EditElectronicReporting = GetFieldWidget("ElectronicReporting");

	SetVisibleField("Auditors", false);
	SetVisibleField("Journals", false);
	SetVisibleField("Seminars", false);
	SetVisibleField("ElectronicReporting", false);
}
//-----------------------------------------------------------------------------
ISScoreObjectForm::~ISScoreObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISScoreObjectForm::CompetitorChanged()
{
	bool Visible = EditCompetitor->GetValue().toBool();
	SetVisibleField("Auditors", Visible);
	SetVisibleField("Journals", Visible);
	SetVisibleField("Seminars", Visible);
	SetVisibleField("ElectronicReporting", Visible);
}
//-----------------------------------------------------------------------------
