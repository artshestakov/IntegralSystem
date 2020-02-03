#include "ISPatriotHelper.h"
#include "ISQuery.h"
#include "ISNotificationService.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QS_VISIT = PREPARE_QUERY("SELECT sbsr_nowexist "
										"FROM subscriptions "
										"WHERE sbsr_id = :SubscriptionID");
//-----------------------------------------------------------------------------
static QString QI_VISIT = PREPARE_QUERY("INSERT INTO visit(vist_subscription) "
										"VALUES(:SubscriptionID) "
										"RETURNING "
										"(SELECT (sbsr_leftcount - 1) as sbsr_leftcount FROM subscriptions WHERE sbsr_id = :SubscriptionID), "
										"(SELECT clts_fio "
										"FROM subscriptions "
										"LEFT JOIN clients ON clts_id = sbsr_client "
										"WHERE sbsr_id = :SubscriptionID), "
										"(SELECT sbtp_name "
										"FROM subscriptions "
										"LEFT JOIN subscriptiontype ON sbtp_id = sbsr_type "
										"WHERE sbsr_id = :SubscriptionID)");
//-----------------------------------------------------------------------------
static QString QU_VISIT = PREPARE_QUERY("UPDATE visit SET "
										"vist_end = CURRENT_TIME, "
										"vist_duration = CURRENT_TIME - vist_begin "
										"WHERE vist_subscription = :SubscriptionID "
										"RETURNING (SELECT clts_fio "
										"FROM subscriptions "
										"LEFT JOIN clients ON clts_id = sbsr_client "
										"WHERE sbsr_id = :SubscriptionID)");
//-----------------------------------------------------------------------------
static QString QU_LEFTCOUNT = PREPARE_QUERY("UPDATE subscriptions SET "
											"sbsr_leftcount = (SELECT sbsr_leftcount -1 FROM subscriptions WHERE sbsr_id = :SubscriptionID) "
											"WHERE sbsr_id = :SubscriptionID");
//-----------------------------------------------------------------------------
static QString QU_NOWEXIST = PREPARE_QUERY("UPDATE subscriptions SET "
										   "sbsr_nowexist = :NowExist "
										   "WHERE sbsr_id = :SubscriptionID");
//-----------------------------------------------------------------------------
bool ISPatriotHelper::IsVisit(int SubscriptionID)
{
	bool NowExist = false;

	ISQuery qSelect(QS_VISIT);
	qSelect.BindValue(":SubscriptionID", SubscriptionID);
	if (qSelect.ExecuteFirst())
	{
		NowExist = qSelect.ReadColumn("sbsr_nowexist").toBool();
	}

	return NowExist;
}
//-----------------------------------------------------------------------------
void ISPatriotHelper::OpenVisit(int SubscriptionID)
{
	ISQuery qInsertVisit(QI_VISIT);
	qInsertVisit.BindValue(":SubscriptionID", SubscriptionID);
	if (qInsertVisit.ExecuteFirst())
	{
		int LeftCount = qInsertVisit.ReadColumn("sbsr_leftcount").toInt();
		QString ClientFIO = qInsertVisit.ReadColumn("clts_fio").toString();
		QString SubscriptionName = qInsertVisit.ReadColumn("sbtp_name").toString();

		SetNowExist(SubscriptionID, true);
		ISNotificationService::ShowNotification(ClientFIO, LANG("Patriot.Notify.OpenVisit").arg(LeftCount));

		if (LeftCount == 1)
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Patriot.Message.Warning.OneVisitLeft").arg(ClientFIO).arg(SubscriptionName));
		}
	}
}
//-----------------------------------------------------------------------------
void ISPatriotHelper::CloseVisit(int SubscriptionID)
{
	ISQuery qUpdateVisit(QU_VISIT);
	qUpdateVisit.BindValue(":SubscriptionID", SubscriptionID);
	if (qUpdateVisit.ExecuteFirst())
	{
		QString ClientFIO = qUpdateVisit.ReadColumn("clts_fio").toString();

		ISQuery qLeftCount(QU_LEFTCOUNT);
		qLeftCount.BindValue(":SubscriptionID", SubscriptionID);
		qLeftCount.Execute();

		SetNowExist(SubscriptionID, false);
		ISNotificationService::ShowNotification(ClientFIO, LANG("Patriot.Notify.CloseVisit"));
	}
}
//-----------------------------------------------------------------------------
void ISPatriotHelper::SetNowExist(int SubscriptionID, bool NowExist)
{
	ISQuery qNowExist(QU_NOWEXIST);
	qNowExist.BindValue(":NowExist", NowExist);
	qNowExist.BindValue(":SubscriptionID", SubscriptionID);
	qNowExist.Execute();
}
//-----------------------------------------------------------------------------
