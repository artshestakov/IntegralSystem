#include "ISCalendarObjectForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
#include "ISDateEdit.h"
#include "ISTimeEdit.h"
//-----------------------------------------------------------------------------
ISCalendarObjectForm::ISCalendarObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	SetVisibleNavigationBar(false);
	SetVisibleFavorites(false);
	SetVisibleDelete(false);
	SetVisibleDeleteCascade(false);
	SetVisibleReRead(false);

	GetTabWidget()->tabBar()->setAutoHide(true);

	if (GetFieldValue("TableName").isValid())
	{
		QAction *ActionCard = new QAction(this);
		ActionCard->setText(LANG("Card"));
		ActionCard->setToolTip(LANG("Card"));
		ActionCard->setIcon(BUFFER_ICONS("Document"));
		connect(ActionCard, &QAction::triggered, this, &ISCalendarObjectForm::OpenCard);
		AddActionToolBar(ActionCard);
	}
}
//-----------------------------------------------------------------------------
ISCalendarObjectForm::~ISCalendarObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISCalendarObjectForm::SetDate(const QDate &Date)
{
	GetFieldWidget("Date")->SetValue(Date);
}
//-----------------------------------------------------------------------------
bool ISCalendarObjectForm::Save()
{
	QDate Date = GetFieldValue("Date").toDate();
	QTime TimeAlert = GetFieldValue("TimeAlert").toTime();
	QDateTime DateTime(Date, TimeAlert);

	if (!Date.isValid())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(LANG("Date")));
		GetFieldWidget("Date")->BlinkRed();
		return false;
	}

	if (!TimeAlert.isValid())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(LANG("TimeAlert")));
		GetFieldWidget("TimeAlert")->BlinkRed();
		return false;
	}

	if (DateTime < QDateTime::currentDateTime())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotCreateCalendarEventPast"));
		return false;
	}

	return ISObjectFormBase::Save();
}
//-----------------------------------------------------------------------------
void ISCalendarObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();
	SetModificationFlag(false);
}
//-----------------------------------------------------------------------------
void ISCalendarObjectForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISCalendarObjectForm::OpenCard()
{
	ISGui::CreateObjectForm(ISNamespace::OFT_Edit, GetFieldValue("TableName").toString(), GetFieldValue("ObjectID").toInt())->show();
}
//-----------------------------------------------------------------------------
