#include "StdAfx.h"
#include "ISTaskHistoryTab.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISPushButton.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_TASK_HISTORY = PREPARE_QUERY("SELECT thst_creationdate, userfullname(thst_user), thac_name, thst_information "
											   "FROM _taskhistory "
											   "LEFT JOIN _taskhistoryaction ON thac_id = thst_action "
											   "WHERE thst_task = :TaskID "
											   "ORDER BY thst_creationdate");
//-----------------------------------------------------------------------------
ISTaskHistoryTab::ISTaskHistoryTab(int task_id, QTabWidget *parent) : ISTaskTabBase(task_id, parent)
{
	ISPushButton *ButtonUpdate = new ISPushButton(this);
	ButtonUpdate->setText(LANG("Update"));
	ButtonUpdate->setIcon(BUFFER_ICONS("Update"));
	ButtonUpdate->setFlat(true);
	connect(ButtonUpdate, &ISPushButton::clicked, this, &ISTaskHistoryTab::Update);
	dynamic_cast<QVBoxLayout*>(layout())->addWidget(ButtonUpdate, 0, Qt::AlignLeft);

	ListWidget = new ISListWidget(this);
	ListWidget->setAlternatingRowColors(true);
	ListWidget->setTextElideMode(Qt::TextElideMode::ElideLeft);
	layout()->addWidget(ListWidget);
}
//-----------------------------------------------------------------------------
ISTaskHistoryTab::~ISTaskHistoryTab()
{

}
//-----------------------------------------------------------------------------
void ISTaskHistoryTab::Update()
{
	ListWidget->Clear();
	ISQuery qSelect(QS_TASK_HISTORY);
	qSelect.BindValue(":TaskID", GetTaskID());
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString CreationDate = qSelect.ReadColumn("thst_creationdate").toDateTime().toString(DATE_TIME_FORMAT_V2);
			QString UserFullName = qSelect.ReadColumn("userfullname").toString();
			QString ActionName = qSelect.ReadColumn("thac_name").toString();
			QString Information = qSelect.ReadColumn("thst_information").toString();

			QVBoxLayout *Layout = new QVBoxLayout();
			Layout->setContentsMargins(MARGINS_LAYOUT_2_PX);

			QWidget *Widget = new QWidget(ListWidget);
			Widget->setLayout(Layout);
			
			QLabel *LabelTitle = new QLabel(Widget);
			LabelTitle->setText(SYMBOL_CIRCLE + SYMBOL_SPACE + CreationDate + " (" + UserFullName + "): " + ActionName);
			LabelTitle->setFont(FONT_TAHOMA_10);
			Layout->addWidget(LabelTitle);

			if (Information.length())
			{
				QLabel *LabelInformation = new QLabel(Widget);
				LabelInformation->setText(Information);
				Layout->addWidget(LabelInformation);
			}

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidget->setItemWidget(ListWidgetItem, Widget);
			
			ISGui::RepaintWidget(Widget);
			ListWidgetItem->setSizeHint(Widget->sizeHint());
		}
	}
}
//-----------------------------------------------------------------------------
