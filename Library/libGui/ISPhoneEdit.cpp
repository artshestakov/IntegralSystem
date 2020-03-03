#include "ISPhoneEdit.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISTelephony.h"
#include "ISNotificationService.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QI_ASTERISK_QUEUE = PREPARE_QUERY("INSERT INTO _asteriskqueue(astq_type, astq_initiated, astq_parameters) "
												 "VALUES((SELECT asqt_id FROM _asteriskqueuetype WHERE asqt_uid = :TypeUID), currentuserid(), :Parameters)");
//-----------------------------------------------------------------------------
ISPhoneEdit::ISPhoneEdit(QWidget *parent) : ISPhoneBaseEdit(parent)
{
	SetVisibleCall(true);
	
	if (!ISTelephony::CheckSetUp())
	{
		SetToolTipCall(LANG("NotSettingTelephonyForCurrentUser"));
		SetCursorCall(CURSOR_FORBIDDEN);
	}
}
//-----------------------------------------------------------------------------
ISPhoneEdit::~ISPhoneEdit()
{

}
//-----------------------------------------------------------------------------
void ISPhoneEdit::Call()
{
	if (ISTelephony::CheckSetUp())
	{
		ISGui::SetWaitGlobalCursor(true);
		
		QString Phone = GetValue().toString();

		QVariantMap VariantMap;
		VariantMap.insert("Phone", Phone);

		ISQuery qInsert(QI_ASTERISK_QUEUE);
		qInsert.BindValue(":TypeUID", CONST_UID_ASTERISK_QUEUE_TYPE_OUT_CALLED);
		qInsert.BindValue(":Parameters", ISSystem::VariantMapToJsonString(VariantMap));
		if (qInsert.Execute())
		{
			ISNotificationService::ShowNotification(LANG("OutcomingCall").arg(Phone));
			emit Called();
		}

		ISGui::SetWaitGlobalCursor(false);
	}
	else
	{
		ISMessageBox::ShowInformation(this, LANG("NotSettingTelephonyForCurrentUser"));
	}
}
//-----------------------------------------------------------------------------
