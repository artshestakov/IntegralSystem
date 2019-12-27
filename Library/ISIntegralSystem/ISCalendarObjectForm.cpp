#include "StdAfx.h"
#include "ISCalendarObjectForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaUser.h"
#include "ISGui.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
#include "ISDateEdit.h"
#include "ISTimeEdit.h"
//-----------------------------------------------------------------------------
ISCalendarObjectForm::ISCalendarObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
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
		ActionCard->setText(LOCALIZATION("Card"));
		ActionCard->setToolTip(LOCALIZATION("Card"));
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
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Error.Field.NullValue").arg(LOCALIZATION("Date")));
		GetFieldWidget("Date")->BlinkRed();
		dynamic_cast<ISDateEdit*>(GetFieldWidget("Date"))->SetCheckEnable(Qt::Checked);
		return false;
	}

	if (!TimeAlert.isValid())
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Error.Field.NullValue").arg(LOCALIZATION("TimeAlert")));
		GetFieldWidget("TimeAlert")->BlinkRed();
		dynamic_cast<ISTimeEdit*>(GetFieldWidget("TimeAlert"))->SetCheckEnable(Qt::Checked);
		return false;
	}

	if (DateTime < QDateTime::currentDateTime())
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotCreateCalendarEventPast"));
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
