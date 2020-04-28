#include "ISHistoryForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtonDialog.h"
#include "ISQuery.h"
#include "ISSettings.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_HISTORY = PREPARE_QUERY("SELECT htry_creationdate, htry_tablename, htry_tablelocalname, htry_objectname, htry_objectid "
										  "FROM _history "
										  "WHERE htry_user = currentuserid() "
										  "ORDER BY htry_id DESC "
										  "LIMIT :Limit OFFSET 0");
//-----------------------------------------------------------------------------
ISHistoryForm::ISHistoryForm(QWidget *parent) : ISInterfaceForm(parent)
{
	setWindowTitle(LANG("History"));
	setWindowIcon(BUFFER_ICONS("History"));

	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	ListWidget = new ISListWidget(this);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISHistoryForm::Open);
	GetMainLayout()->addWidget(ListWidget);

	ISQuery qSelectHistory(QS_HISTORY);
	qSelectHistory.BindValue(":Limit", SETTING_STRING(CONST_UID_SETTING_OTHER_MAXIMUMVIEWHISTORY));
	if (qSelectHistory.Execute())
	{
		while (qSelectHistory.Next())
		{
			QString DateTimeOpened = qSelectHistory.ReadColumn("htry_creationdate").toDateTime().toString(FORMAT_DATE_TIME_V3);
			QString TableName = qSelectHistory.ReadColumn("htry_tablename").toString();
			QString TableLocalName = qSelectHistory.ReadColumn("htry_tablelocalname").toString();
			QString ObjectName = qSelectHistory.ReadColumn("htry_objectname").toString();
			int ObjectID = qSelectHistory.ReadColumn("htry_objectid").toInt();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(TableLocalName + ": " + ObjectName + " (" + DateTimeOpened + ')');
			ListWidgetItem->setData(Qt::UserRole, TableName);
			ListWidgetItem->setData(Qt::UserRole * 2, ObjectID);
		}
	}

	ISButtonDialog *ButtonDialog = new ISButtonDialog(this);
	ButtonDialog->SetApplyVisible(false);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISHistoryForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISHistoryForm::~ISHistoryForm()
{

}
//-----------------------------------------------------------------------------
void ISHistoryForm::Open()
{
	QListWidgetItem *ListWidgetItem = ListWidget->currentItem();
	emit OpenObject(ListWidgetItem->data(Qt::UserRole).toString(), ListWidgetItem->data(Qt::UserRole * 2).toInt());
}
//-----------------------------------------------------------------------------