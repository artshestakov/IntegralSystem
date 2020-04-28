#include "ISNotifyRecipient.h"
#include "ISDatabase.h"
#include "ISSystem.h"
#include "ISBuffer.h"
#include "ISNotificationService.h"
#include "ISMediaPlayer.h"
#include "ISStructs.h"
#include "ISAssert.h"
#include "ISNotifySender.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISNotifyRecipient::ISNotifyRecipient() : QObject()
{
	connect(ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).driver(),
		static_cast<void(QSqlDriver::*)(const QString &, QSqlDriver::NotificationSource, const QVariant &)>(&QSqlDriver::notification), this, &ISNotifyRecipient::Notification);
}
//-----------------------------------------------------------------------------
ISNotifyRecipient::~ISNotifyRecipient()
{

}
//-----------------------------------------------------------------------------
ISNotifyRecipient& ISNotifyRecipient::GetInstance()
{
	static ISNotifyRecipient NotifyRecipient;
	return NotifyRecipient;
}
//-----------------------------------------------------------------------------
void ISNotifyRecipient::Notification(const ISUuid &NotificationName, QSqlDriver::NotificationSource Source, const QVariant &Payload)
{
	QVariantMap VariantMap = ISSystem::JsonStringToVariantMap(Payload.toString());
	ISUuid NotificationUID = VariantMap.value("NotificationUID");
	QString PayloadString = VariantMap.value("Payload").toString();
	QString String = VariantMap.value("String").toString();

	ISMetaNotify *MetaNotify = ISNotifySender::GetInstance().GetMetaNotify(NotificationUID);
	if (!MetaNotify->SoundFileName.isEmpty()) //���� � ����������� ������ �������� ������
	{
		ISMediaPlayer::GetInstance().Play(BUFFER_AUDIO(MetaNotify->SoundFileName));
	}

	QString NotificationText = MetaNotify->MessageNotify;
	if (String.length())
	{
		NotificationText += ":\n\n" + String;
	}

	QString SignalName = MetaNotify->SignalName;
	if (SignalName.length()) //���� ������ ������������ ������ ��� �����������
	{
		bool Invoked = QMetaObject::invokeMethod(this, SignalName.toUtf8().data(), Q_ARG(const QVariantMap &, VariantMap));
		IS_ASSERT(Invoked, QString("Not invoke signal \"%1\" in notification \"%2\"").arg(SignalName).arg(MetaNotify->UID));
	}
	else //������������ ������ ��� ����������� �� ������ - ��������� ����� �������
	{
		emit Notify(VariantMap);
		emit Notify();
	}

	if (MetaNotify->ShowPopup)
	{
		ISNotificationService::ShowNotification(ISGui::PrepareLongToolTip(NotificationText, 350));
	}
}
//-----------------------------------------------------------------------------