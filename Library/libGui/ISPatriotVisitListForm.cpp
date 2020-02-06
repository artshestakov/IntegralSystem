#include "ISPatriotVisitListForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISObjects.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QD_CANCEL_VISIT = PREPARE_QUERY("DELETE FROM visit "
											   "WHERE vist_id = :ObjectID");
//-----------------------------------------------------------------------------
ISPatriotVisitListForm::ISPatriotVisitListForm(QWidget *parent) : ISListBaseForm("Visit", parent)
{
	QAction *ActionCancelVisit = new QAction(this);
	ActionCancelVisit->setText(LANG("Patriot.CancelVisit"));
	ActionCancelVisit->setToolTip(LANG("Patriot.CancelVisit"));
	ActionCancelVisit->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("VisitClose"));
	connect(ActionCancelVisit, &QAction::triggered, this, &ISPatriotVisitListForm::CancelVisit);
	AddAction(ActionCancelVisit, true, true);
}
//-----------------------------------------------------------------------------
ISPatriotVisitListForm::~ISPatriotVisitListForm()
{

}
//-----------------------------------------------------------------------------
void ISPatriotVisitListForm::CancelVisit()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Patriot.Message.Question.CancelVisit")))
	{
		ISQuery qDelete(QD_CANCEL_VISIT);
		qDelete.BindValue(":ObjectID", GetObjectID());
		if (qDelete.Execute())
		{
			ISMessageBox::ShowInformation(this, LANG("Patriot.Message.Information.CancelVisit"));
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
