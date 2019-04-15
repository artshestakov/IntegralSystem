#include "StdAfx.h"
#include "ISPatriotClientsBirthdayForm.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtonClose.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_CLIENT = PREPARE_QUERY("SELECT clts_fio, cltp_name, EXTRACT(YEAR FROM CURRENT_DATE) - EXTRACT(YEAR FROM clts_birthday) as YearCount "
										 "FROM clients "
										 "LEFT JOIN clienttype ON cltp_id = clts_type "
										 "WHERE clts_id = :ClientID");
//-----------------------------------------------------------------------------
ISPatriotClientsBirthdayForm::ISPatriotClientsBirthdayForm(QWidget *parent) : ISInterfaceForm(parent)
{
	setWindowTitle(LOCALIZATION("Patriot.Birthdays"));
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_5_PX);

	GetMainLayout()->addWidget(new QLabel(LOCALIZATION("Patriot.BirthdayCurrentDate") + ":", this));

	ListWidget = new ISListWidget(this);
	ListWidget->setAlternatingRowColors(true);
	GetMainLayout()->addWidget(ListWidget);

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISPatriotClientsBirthdayForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISPatriotClientsBirthdayForm::~ISPatriotClientsBirthdayForm()
{

}
//-----------------------------------------------------------------------------
void ISPatriotClientsBirthdayForm::AddClient(int ClientID)
{
	ISQuery qSelectClient(QS_CLIENT);
	qSelectClient.BindValue(":ClientID", ClientID);
	if (qSelectClient.ExecuteFirst())
	{
		QString FIO = qSelectClient.ReadColumn("clts_fio").toString();
		QString Type = qSelectClient.ReadColumn("cltp_name").toString();
		int YearCount = qSelectClient.ReadColumn("YearCount").toInt();
		
		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
		ListWidgetItem->setText(FIO + " (" + Type + "). " + LOCALIZATION("Patriot.WasFulfilled").arg(YearCount));
		ListWidgetItem->setIcon(BUFFER_ICONS("Birthday"));
		ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
	}
}
//-----------------------------------------------------------------------------
