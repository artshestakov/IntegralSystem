#include "ISCalendarObjectForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISCalendarObjectForm::ISCalendarObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	SetVisibleNavigationBar(false);
	SetVisibleFavorites(false);
	SetVisibleDelete(false);
}
//-----------------------------------------------------------------------------
ISCalendarObjectForm::~ISCalendarObjectForm()
{

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
