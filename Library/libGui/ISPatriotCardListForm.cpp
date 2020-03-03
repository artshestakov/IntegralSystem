#include "ISPatriotCardListForm.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISInputDialog.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
static QString QU_DEACTIVATE = PREPARE_QUERY2("UPDATE card SET "
											 "card_active = false, "
											 "card_reason = :Reason "
											 "WHERE card_id = :ObjectID");
//-----------------------------------------------------------------------------
ISPatriotCardListForm::ISPatriotCardListForm(QWidget *parent) : ISListBaseForm("Card", parent)
{
	QAction *ActionDeactivate = new QAction(this);
	ActionDeactivate->setText(LANG("Patriot.DeactivateCard"));
	ActionDeactivate->setToolTip(LANG("Patriot.DeactivateCard"));
	ActionDeactivate->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("DeactivateCard"));
	connect(ActionDeactivate, &QAction::triggered, this, &ISPatriotCardListForm::Deactivate);
	AddAction(ActionDeactivate, true, true);
}
//-----------------------------------------------------------------------------
ISPatriotCardListForm::~ISPatriotCardListForm()
{

}
//-----------------------------------------------------------------------------
void ISPatriotCardListForm::Deactivate()
{
	QString Barcode = GetCurrentRecordValue("Barcode").toString();
	if (ISMessageBox::ShowQuestion(this, LANG("Patriot.Message.Question.DeactivateCard").arg(Barcode)))
	{
		QVariant Reason = ISInputDialog::GetText(this, LANG("Patriot.Deactivating"), LANG("Patriot.DeactivatingReason") + ':');

		ISQuery qDeactivate(QU_DEACTIVATE);
		qDeactivate.BindValue(":Reason", Reason);
		qDeactivate.BindValue(":ObjectID", GetObjectID());
		if (qDeactivate.Execute())
		{
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
