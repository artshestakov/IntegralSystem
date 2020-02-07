#include "ISNotifyWidgetItem.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISServiceButton.h"
#include "ISNotifySender.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISNotifyWidgetItem::ISNotifyWidgetItem(QWidget *parent, int notification_user_id, const ISUuid &NotificationUID, const QDateTime &DateTime, const QString &PayloadString) : QWidget(parent)
{
	ID = notification_user_id;

	QHBoxLayout *Layout = new QHBoxLayout();
	Layout->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_5_PX);
	setLayout(Layout);

	QVBoxLayout *LayoutLeft = new QVBoxLayout();
	Layout->addLayout(LayoutLeft);

	QLabel *LabelDateTime = new QLabel(this);
	LabelDateTime->setText(ISGui::ConvertDateTimeToString(DateTime, DATE_FORMAT_V2, TIME_FORMAT_V1));
	LabelDateTime->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
	LayoutLeft->addWidget(LabelDateTime, 0, Qt::AlignLeft);
	
	QLabel *LabelMessage = new QLabel(this);
	LabelMessage->setText(ISNotifySender::GetInstance().GetMetaNotify(NotificationUID)->MessageNotify);
	LabelMessage->setWordWrap(true);
	LayoutLeft->addWidget(LabelMessage);

	if (PayloadString.length())
	{
		LabelMessage->setText(LabelMessage->text() + "\n" + PayloadString);
	}

	ISServiceButton *ButtonDelete = new ISServiceButton(this);
	ButtonDelete->setIcon(BUFFER_ICONS("Exit"));
	ButtonDelete->setToolTip(LANG("DeleteNotification"));
	ButtonDelete->setFlat(true);
	connect(ButtonDelete, &ISServiceButton::clicked, this, &ISNotifyWidgetItem::DeleteClicked);
	Layout->addWidget(ButtonDelete);
}
//-----------------------------------------------------------------------------
ISNotifyWidgetItem::~ISNotifyWidgetItem()
{

}
//-----------------------------------------------------------------------------
void ISNotifyWidgetItem::DeleteClicked()
{
	emit Delete(ID);
}
//-----------------------------------------------------------------------------
