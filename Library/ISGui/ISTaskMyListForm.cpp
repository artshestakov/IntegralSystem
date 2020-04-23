#include "ISTaskMyListForm.h"
#include "ISDefinesCore.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISConstants.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
static QString QS_TASK_STATUS = PREPARE_QUERY("SELECT tsst_uid, tsst_name "
											  "FROM _taskstatus "
											  "ORDER BY tsst_order");
//-----------------------------------------------------------------------------
ISTaskMyListForm::ISTaskMyListForm(QWidget *parent) : ISTaskBaseListForm(parent)
{
	GetQueryModel()->SetClassFilter("task_executor = currentuserid()");
	GetToolBar()->addWidget(new QLabel(LANG("Filter") + ':', this));

	ActionFilter = new QAction(this);
	ActionFilter->setText(LANG("All"));
	ActionFilter->setIcon(BUFFER_ICONS("Filter"));
	ActionFilter->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	ActionFilter->setMenu(new QMenu(this));
	AddAction(ActionFilter, false);

	QToolButton *ToolButton = dynamic_cast<QToolButton*>(GetToolBar()->widgetForAction(ActionFilter));
	ToolButton ->setPopupMode(QToolButton::InstantPopup);
	ToolButton ->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ToolButton ->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));
	ToolButton->setCursor(CURSOR_POINTING_HAND);

	ISQuery qSelectStatus(QS_TASK_STATUS);
	if (qSelectStatus.Execute())
	{
		QAction *ActionAll = CreateActionFilter(LANG("All"), ISUuid());
		ActionAll->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		ActionAll->setChecked(true);

		while (qSelectStatus.Next())
		{
			ISUuid StatusUID = qSelectStatus.ReadColumn("tsst_uid");
			QString StatusName = qSelectStatus.ReadColumn("tsst_name").toString();
			CreateActionFilter(StatusName, StatusUID);
		}
	}
}
//-----------------------------------------------------------------------------
ISTaskMyListForm::~ISTaskMyListForm()
{

}
//-----------------------------------------------------------------------------
QAction* ISTaskMyListForm::CreateActionFilter(const QString &StatusName, const ISUuid &StatusUID)
{
	QAction *Action = new QAction(ActionFilter->menu());
	Action->setText(StatusName);
	Action->setData(StatusUID);
	Action->setCheckable(true);
	connect(Action, &QAction::triggered, this, &ISTaskMyListForm::FilterChanged);
	ActionFilter->menu()->addAction(Action);
	return Action;
}
//-----------------------------------------------------------------------------
void ISTaskMyListForm::FilterChanged()
{
	QAction *ActionSender = dynamic_cast<QAction*>(sender());

	for (QAction *Action : ActionFilter->menu()->actions())
	{
		Action->setFont(ISDefines::Gui::FONT_APPLICATION);
		Action->setChecked(false);

		if (Action == ActionSender)
		{
			Action->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
			Action->setChecked(true);
		}
	}
	
	QString StatusName = ActionSender->text();
	ISUuid StatusUID = ActionSender->data();

	ActionFilter->setText(ActionSender->text());

	if (StatusUID.IsValid())
	{
		GetQueryModel()->SetClassFilter("task_executor = currentuserid() AND task_status = (SELECT tsst_id FROM _taskstatus WHERE tsst_uid = '" + StatusUID + "')");
	}
	else
	{
		GetQueryModel()->SetClassFilter("task_executor = currentuserid()");
	}

	Update();
}
//-----------------------------------------------------------------------------
