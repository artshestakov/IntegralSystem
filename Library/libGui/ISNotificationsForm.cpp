#include "ISNotificationsForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISNotifyWidgetItem.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISNotificationService.h"
#include "ISNotifySender.h"
//-----------------------------------------------------------------------------
static QString QS_NOTIFICATION = PREPARE_QUERY("SELECT ntfu_id, ntfn_uid, ntfu_creationdate, ntfu_payload "
											   "FROM _notificationuser "
											   "LEFT JOIN _notification ON ntfn_id = ntfu_notificationid "
											   "WHERE ntfu_userto = currentuserid() "
											   "ORDER BY ntfu_creationdate DESC");
//-----------------------------------------------------------------------------
static QString QD_NOTIFICATION = PREPARE_QUERY("DELETE FROM _notificationuser WHERE ntfu_id = :NotificationUserID");
//-----------------------------------------------------------------------------
static QString QD_NOTIFICATION_CLEAR = PREPARE_QUERY("DELETE FROM _notificationuser WHERE ntfu_userto = currentuserid()");
//-----------------------------------------------------------------------------
ISNotificationsForm::ISNotificationsForm(QWidget *parent) : ISInterfaceForm(parent)
{
	setWindowFlags(Qt::Popup);
	
	QVBoxLayout *LayoutFrame = new QVBoxLayout();
	LayoutFrame->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_5_PX);

	QFrame *Frame = new QFrame(this);
	Frame->setFrameShape(QFrame::Box);
	Frame->setFrameShadow(QFrame::Raised);
	Frame->setLayout(LayoutFrame);
	GetMainLayout()->addWidget(Frame);

	LayoutFrame->addWidget(new QLabel(LANG("Notifications") + ':', this));

	ListWidget = new ISListWidget(this);
	ListWidget->setVerticalScrollMode(ISListWidget::ScrollPerPixel);
	LayoutFrame->addWidget(ListWidget);

	LayoutFrame->addWidget(ISControls::CreateHorizontalLine(this));

	QHBoxLayout *LayoutButtons = new QHBoxLayout();
	LayoutButtons->addStretch();
	LayoutFrame->addLayout(LayoutButtons);

	ButtonClear = new ISPushButton(this);
	ButtonClear->setEnabled(false);
	ButtonClear->setText(LANG("Clear"));
	connect(ButtonClear, &ISPushButton::clicked, this, &ISNotificationsForm::Clear);
	LayoutButtons->addWidget(ButtonClear);

	ISPushButton *ButtonHide = new ISPushButton(this);
	ButtonHide->setText(LANG("Hide"));
	connect(ButtonHide, &ISPushButton::clicked, [=] { HideAnimation(350); });
	LayoutButtons->addWidget(ButtonHide);

	LabelEmpty = new QLabel(this);
	LabelEmpty->setVisible(false);
	LabelEmpty->setText(LANG("NotificationEmpty") + "\n" + LANG("NotificationEmpty.DetailedText"));
	LabelEmpty->setWordWrap(true);
	LabelEmpty->setFont(DEFINES_GUI.FONT_TAHOMA_10);
	LabelEmpty->setAlignment(Qt::AlignCenter);
	LabelEmpty->adjustSize();

	ReloadNotify();
}
//-----------------------------------------------------------------------------
ISNotificationsForm::~ISNotificationsForm()
{

}
//-----------------------------------------------------------------------------
void ISNotificationsForm::paintEvent(QPaintEvent *e)
{
	QWidget::paintEvent(e);

	if (LabelEmpty->isVisible())
	{
		QRect Rect = ListWidget->frameGeometry();
		QPoint CenterPoint = Rect.center();
		CenterPoint.setX(CenterPoint.x() - (LabelEmpty->width() / 2));
		CenterPoint.setY(CenterPoint.y() - (LabelEmpty->height() / 2));
		LabelEmpty->move(CenterPoint);
	}
}
//-----------------------------------------------------------------------------
void ISNotificationsForm::ReloadNotify()
{
	ListWidget->Clear();
	ISQuery qSelect(QS_NOTIFICATION);
	if (qSelect.Execute())
	{
		if (qSelect.GetCountResultRows()) //Если есть уведомлений - заполнить список
		{
			LabelEmpty->setVisible(false);
			ButtonClear->setEnabled(true);

			while (qSelect.Next())
			{
				int ID = qSelect.ReadColumn("ntfu_id").toInt();
				ISUuid NotificationUID = qSelect.ReadColumn("ntfn_uid");
				QDateTime DateTime = qSelect.ReadColumn("ntfu_creationdate").toDateTime();
				QString PayloadString = qSelect.ReadColumn("ntfu_payload").toString();

				ISNotifyWidgetItem *NotifyWidgetItem = new ISNotifyWidgetItem(this, ID, NotificationUID, DateTime, PayloadString);
				connect(NotifyWidgetItem, &ISNotifyWidgetItem::Delete, this, &ISNotificationsForm::DeleteNotifyUser);

				QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
				ListWidgetItem->setSizeHint(NotifyWidgetItem->sizeHint());
				ListWidget->setItemWidget(ListWidgetItem, NotifyWidgetItem);
			}
		}
		else //Уведомления отсутствуют
		{
			LabelEmpty->setVisible(true);
			ButtonClear->setEnabled(false);
		}
	}
}
//-----------------------------------------------------------------------------
void ISNotificationsForm::Clear()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ClearListNotifications")))
	{
		ISQuery qClear(QD_NOTIFICATION_CLEAR);
		if (qClear.Execute())
		{
			ReloadNotify();
			emit CountChanged(ListWidget->count());
		}
	}
}
//-----------------------------------------------------------------------------
void ISNotificationsForm::DeleteNotifyUser(int ID)
{
	ISQuery qDelete(QD_NOTIFICATION);
	qDelete.BindValue(":NotificationUserID", ID);
	if (qDelete.Execute())
	{
		ReloadNotify();
		emit CountChanged(ListWidget->count());
	}
}
//-----------------------------------------------------------------------------
