#include "ISPatriotSubscriptionSelectForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISPushButton.h"
#include "ISButtonClose.h"
#include "ISPatriotHelper.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_SUBSCRIPTION = PREPARE_QUERY("SELECT sbsr_id, sbtp_name, sbtp_count, sbsr_leftcount, sbsr_nowexist "
											   "FROM subscriptions "
											   "LEFT JOIN subscriptiontype ON sbtp_id = sbsr_type "
											   "WHERE NOT sbsr_isdeleted "
											   "AND sbsr_leftcount != 0 "
											   "AND sbsr_client = (SELECT card_client FROM card WHERE card_barcode = :Barcode) "
											   "ORDER BY sbsr_id");
//-----------------------------------------------------------------------------
ISPatriotSubscriptionSelectForm::ISPatriotSubscriptionSelectForm(const QString &Barcode, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("Patriot.SubscriptionSelect"));
	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_5_PX);

	GetMainLayout()->addWidget(new QLabel(LANG("Patriot.SelectSebscriptionClientForVisit") + ':', this));

	ButtonGroup = new QButtonGroup(this);

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);
	GetMainLayout()->addStretch();

	ISQuery qSelect(QS_SUBSCRIPTION);
	qSelect.BindValue(":Barcode", Barcode);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			int ID = qSelect.ReadColumn("sbsr_id").toInt();
			QString Name = qSelect.ReadColumn("sbtp_name").toString();
			int Count = qSelect.ReadColumn("sbtp_count").toInt();
			int LeftCount = qSelect.ReadColumn("sbsr_leftcount").toInt();
			bool NowExist = qSelect.ReadColumn("sbsr_nowexist").toBool();

			QLabel *Label = new QLabel(this);
			Label->setText(Name + SYMBOL_SPACE + LANG("Patriot.SubscriptionForVisits").arg(Count).arg(LeftCount));

			ISPushButton *PushButton = new ISPushButton(this);
			PushButton->setSizePolicy(QSizePolicy::Maximum, PushButton->sizePolicy().verticalPolicy());
			
			if (NowExist)
			{
				Label->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
				PushButton->setText(LANG("Patriot.CloseVisit"));
				connect(PushButton, &ISPushButton::clicked, this, &ISPatriotSubscriptionSelectForm::CloseVisit);
			}
			else
			{
				PushButton->setText(LANG("Patriot.OpenVisit"));
				connect(PushButton, &ISPushButton::clicked, this, &ISPatriotSubscriptionSelectForm::OpenVisit);
			}
			
			FormLayout->addRow(Label, PushButton);
			ButtonGroup->addButton(PushButton, ID);
		}
	}

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISPatriotSubscriptionSelectForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISPatriotSubscriptionSelectForm::~ISPatriotSubscriptionSelectForm()
{

}
//-----------------------------------------------------------------------------
void ISPatriotSubscriptionSelectForm::OpenVisit()
{
	ISPatriotHelper::OpenVisit(ButtonGroup->id(dynamic_cast<QAbstractButton*>(sender())));
	close();
}
//-----------------------------------------------------------------------------
void ISPatriotSubscriptionSelectForm::CloseVisit()
{
	ISPatriotHelper::CloseVisit(ButtonGroup->id(dynamic_cast<QAbstractButton*>(sender())));
	close();
}
//-----------------------------------------------------------------------------
