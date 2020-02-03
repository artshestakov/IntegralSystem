#include "ISPatriot.h"
#include "ISQuery.h"
#include "ISPatriotClientsBirthdayForm.h"
#include "ISDeviceEntity.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISPatriotSubscriptionSelectForm.h"
#include "ISPatriotHelper.h"
//-----------------------------------------------------------------------------
#include "ISPatriotSubscriptionListForm.h"
#include "ISPatriotSubscriptionObjectForm.h"
#include "ISPatriotCardListForm.h"
#include "ISPatriotTrainerReportForm.h"
#include "ISPatriotVisitListForm.h"
//-----------------------------------------------------------------------------
static QString QS_CLIENT_BIRTHDAY = PREPARE_QUERY("SELECT clts_id "
												  "FROM clients "
												  "WHERE NOT clts_isdeleted "
												  "AND EXTRACT(DAY FROM clts_birthday) = EXTRACT(DAY FROM CURRENT_DATE) "
												  "AND EXTRACT(MONTH FROM clts_birthday) = EXTRACT(MONTH FROM CURRENT_DATE) "
												  "ORDER BY clts_fio");
//-----------------------------------------------------------------------------
static QString QS_CARD_ACTIVE = PREPARE_QUERY("SELECT card_active "
											  "FROM card "
											  "WHERE NOT card_isdeleted "
											  "AND card_barcode = :Barcode");
//-----------------------------------------------------------------------------
static QString QS_CARD = PREPARE_QUERY("SELECT COUNT(*) "
									   "FROM card "
									   "WHERE NOT card_isdeleted "
									   "AND card_barcode = :Barcode "
									   "AND card_active");
//-----------------------------------------------------------------------------
static QString QS_CLIENT = PREPARE_QUERY("SELECT clts_id, clts_fio, (SELECT COUNT(*) FROM subscriptions WHERE NOT sbsr_isdeleted AND sbsr_leftcount != 0 AND sbsr_client = clts_id) "
										 "FROM clients "
										 "WHERE clts_id = (SELECT card_client FROM card WHERE card_barcode = :Barcode)");
//-----------------------------------------------------------------------------
static QString QS_SUBSCRIPTION = PREPARE_QUERY("SELECT sbsr_id "
											   "FROM subscriptions "
											   "WHERE NOT sbsr_isdeleted "
											   "AND sbsr_leftcount != 0 "
											   "AND sbsr_client = :ClientID");
//-----------------------------------------------------------------------------
ISPatriot::ISPatriot(QObject *parent) : ISObjectInterface(parent)
{

}
//-----------------------------------------------------------------------------
ISPatriot::~ISPatriot()
{

}
//-----------------------------------------------------------------------------
void ISPatriot::RegisterMetaTypes() const
{
	qRegisterMetaType<ISPatriotSubscriptionListForm*>("ISPatriotSubscriptionListForm");
	qRegisterMetaType<ISPatriotSubscriptionObjectForm*>("ISPatriotSubscriptionObjectForm");
	qRegisterMetaType<ISPatriotCardListForm*>("ISPatriotCardListForm");
	qRegisterMetaType<ISPatriotTrainerReportForm*>("ISPatriotTrainerReportForm");
	qRegisterMetaType<ISPatriotVisitListForm*>("ISPatriotVisitListForm");
}
//-----------------------------------------------------------------------------
void ISPatriot::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISPatriot::InitializePlugin() const
{
	ISQuery qSelect(QS_CLIENT_BIRTHDAY);
	if (qSelect.Execute())
	{
		if (qSelect.GetCountResultRows())
		{
			ISPatriotClientsBirthdayForm *ClientsBirthdayForm = new ISPatriotClientsBirthdayForm();

			while (qSelect.Next())
			{
				ClientsBirthdayForm->AddClient(qSelect.ReadColumn("clts_id").toInt());
			}

			ClientsBirthdayForm->show();
		}
	}

	connect(&ISDeviceEntity::GetInstance(), &ISDeviceEntity::InputData, this, &ISPatriot::InputScannerData);
}
//-----------------------------------------------------------------------------
void ISPatriot::InputScannerData(const QString &Barcode)
{
	if (!IsActiveCard(Barcode))
	{
		ISMessageBox::ShowWarning(nullptr, LANG("Patriot.Message.Warning.CardDeactivate").arg(Barcode));
		return;
	}

	if (SearchCard(Barcode)) //Если карта найдена
	{
		ISQuery qSelectClient(QS_CLIENT);
		qSelectClient.BindValue(":Barcode", Barcode);
		if (qSelectClient.ExecuteFirst())
		{
			int ClientID = qSelectClient.ReadColumn("clts_id").toInt();
			QString ClientFIO = qSelectClient.ReadColumn("clts_fio").toString();
			int CountSubscriptions = qSelectClient.ReadColumn("count").toInt();

			if (CountSubscriptions == 0) //Если абонементы отсутствуют у клиента
			{
				if (ISMessageBox::ShowQuestion(nullptr, LANG("Patriot.Message.Question.CreateSubscription").arg(ClientFIO)))
				{
					ISPatriotSubscriptionObjectForm *SubscriptionObjectForm = dynamic_cast<ISPatriotSubscriptionObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_New, "Subscriptions"));
					SubscriptionObjectForm->SetClient(ClientID);
					SubscriptionObjectForm->show();
				}
			}
			else if (CountSubscriptions == 1) //Если у клиента абонемент один
			{
				ISQuery qSelectSubscription(QS_SUBSCRIPTION);
				qSelectSubscription.BindValue(":ClientID", ClientID);
				if (qSelectSubscription.ExecuteFirst())
				{
					int SubscriptionID = qSelectSubscription.ReadColumn("sbsr_id").toInt();
					if (ISPatriotHelper::IsVisit(SubscriptionID))
					{
						ISPatriotHelper::CloseVisit(SubscriptionID);
					}
					else
					{
						ISPatriotHelper::OpenVisit(SubscriptionID);
					}
				}
			}
			else if (CountSubscriptions > 1) //Если у клиента больше одного абонемента
			{
				ISPatriotSubscriptionSelectForm SubscriptionSelectForm(Barcode);
				SubscriptionSelectForm.Exec();
			}
		}
	}
	else //Карта не найдена - предложить привязать её к клиенту
	{
		if (ISMessageBox::ShowQuestion(nullptr, LANG("Patriot.Message.Question.CreateCard").arg(Barcode))) //Если нужно привязать карту к клиенту
		{
			ISObjectFormBase *CardObjectForm = ISGui::CreateObjectForm(ISNamespace::OFT_New, "Card");
			CardObjectForm->SetFieldValue("Barcode", Barcode);
			CardObjectForm->show();
		}
	}
}
//-----------------------------------------------------------------------------
bool ISPatriot::IsActiveCard(const QString &Barcode) const
{
	bool Result = true;
	ISQuery qSelect(QS_CARD_ACTIVE);
	qSelect.BindValue(":Barcode", Barcode);
	if (qSelect.ExecuteFirst())
	{
		Result = qSelect.ReadColumn("card_active").toBool();
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISPatriot::SearchCard(const QString &Barcode) const
{
	ISQuery qSelectCard(QS_CARD);
	qSelectCard.BindValue(":Barcode", Barcode);
	if (qSelectCard.ExecuteFirst())
	{
		if (qSelectCard.ReadColumn("count").toInt())
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
