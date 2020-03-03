#include "ISPatriotSubscriptionObjectForm.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_DISCOUNT = PREPARE_QUERY2("SELECT (SELECT dcnt_percent FROM discount WHERE dcnt_id = clts_discount) "
										   "FROM subscriptions "
										   "LEFT JOIN clients ON clts_id = sbsr_client "
										   "WHERE sbsr_id = :ID");
//-----------------------------------------------------------------------------
static QString QS_LEFT_COUNT = PREPARE_QUERY2("SELECT sbtp_count, sbtp_cost, (sbtp_cost - (sbtp_cost * dcnt_percent / 100)) AS price "
											 "FROM subscriptions "
											 "LEFT JOIN subscriptiontype ON sbtp_id = sbsr_type "
											 "LEFT JOIN clients ON clts_id = sbsr_client "
											 "LEFT JOIN discount ON dcnt_id = clts_discount "
											 "WHERE sbsr_id = :SubscriptionID");
//-----------------------------------------------------------------------------
static QString QU_SUBSCRIBER = PREPARE_QUERY2("UPDATE subscriptions SET "
											 "sbsr_leftcount = :LeftCount, "
											 "sbsr_price = :Price "
											 "WHERE sbsr_id = :SubscriptionID");
//-----------------------------------------------------------------------------
ISPatriotSubscriptionObjectForm::ISPatriotSubscriptionObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISPatriotSubscriptionObjectForm::~ISPatriotSubscriptionObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISPatriotSubscriptionObjectForm::SetClient(int ClientID)
{
	GetFieldWidget("Client")->SetValue(ClientID);
	GetFieldWidget("Client")->setEnabled(false);
}
//-----------------------------------------------------------------------------
bool ISPatriotSubscriptionObjectForm::Save()
{
	ISNamespace::ObjectFormType BeginFormType = GetFormType();
	bool Result = ISObjectFormBase::Save();
	if (Result)
	{
		if (BeginFormType == ISNamespace::OFT_New || BeginFormType == ISNamespace::OFT_Copy)
		{
			ISQuery qSelectCount(QS_LEFT_COUNT);
			qSelectCount.BindValue(":SubscriptionID", GetObjectID());
			if (qSelectCount.ExecuteFirst())
			{
				int LeftCount = qSelectCount.ReadColumn("sbtp_count").toInt();
				double Price = qSelectCount.ReadColumn("sbtp_cost").toDouble();
				double Discount = GetDiscount(GetObjectID());

				if (Discount) //Если у клиента указана скидка
				{
					double Temp = (Price * Discount) / 100;
					Price = Price - Temp;
				}

				ISQuery qUpdate(QU_SUBSCRIBER);
				qUpdate.BindValue(":LeftCount", LeftCount);
				qUpdate.BindValue(":Price", Price);
				qUpdate.BindValue(":SubscriptionID", GetObjectID());
				qUpdate.Execute();
			}
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
double ISPatriotSubscriptionObjectForm::GetDiscount(int id) const
{
	double Result = 0;

	ISQuery qSelect(QS_DISCOUNT);
	qSelect.BindValue(":ID", id);
	if (qSelect.ExecuteFirst())
	{
		Result = qSelect.ReadColumn("dcnt_percent").toDouble();
	}

	return Result;
}
//-----------------------------------------------------------------------------
