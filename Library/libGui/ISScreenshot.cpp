#include "ISScreenshot.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISGui.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISNotifySender.h"
//-----------------------------------------------------------------------------
static QString QI_SCREENSHOT = PREPARE_QUERY("INSERT INTO _screenshots(scrn_screenshot) "
											 "VALUES(:Screenshot) "
											 "RETURNING scrn_id");
//-----------------------------------------------------------------------------
ISScreenshot::ISScreenshot() : QObject()
{
	FutureWatcher = new QFutureWatcher<void>();
	Database = QSqlDatabase::cloneDatabase(ISDatabase::GetInstance().GetDefaultDB(), "test");
}
//-----------------------------------------------------------------------------
ISScreenshot::~ISScreenshot()
{

}
//-----------------------------------------------------------------------------
ISScreenshot& ISScreenshot::GetInstance()
{
	static ISScreenshot Screenshot;
	return Screenshot;
}
//-----------------------------------------------------------------------------
void ISScreenshot::Create(const QVariantMap &VariantMap)
{
	if (!FutureWatcher->isRunning())
	{
		QFuture<void> Future = QtConcurrent::run(this, &ISScreenshot::Creating, VariantMap.value("Payload").toInt());
		FutureWatcher->setFuture(Future);
	}
}
//-----------------------------------------------------------------------------
void ISScreenshot::Creating(int Owner)
{
	if (Database.open())
	{
		{
			WId WinID = QApplication::desktop()->winId();
			QPixmap Pixmap = QApplication::primaryScreen()->grabWindow(WinID);
			QByteArray ByteArray = ISGui::PixmapToByteArray(Pixmap);

			ISQuery qInsertScreenshot(Database, QI_SCREENSHOT);
			qInsertScreenshot.BindValue(":Screenshot", ByteArray);
			if (qInsertScreenshot.ExecuteFirst())
			{
				ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_SCREENSHOT_CREATED, Owner, qInsertScreenshot.ReadColumn("scrn_id"), QString(), false);
			}
		}
		Database.close();
	}
}
//-----------------------------------------------------------------------------
