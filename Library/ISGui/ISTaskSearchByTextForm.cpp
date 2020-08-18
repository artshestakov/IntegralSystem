#include "ISTaskSearchByTextForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefinesGui.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_SEARCH = PREPARE_QUERY("WITH w AS "
										 "( "
										 "SELECT task_id, task_parent, lower(task_name) AS search_field "
										 "FROM _task "
										 "WHERE NOT task_isdeleted "
										 "UNION "
										 "SELECT task_id, task_parent, lower(task_description) AS search_field "
										 "FROM _task "
										 "WHERE NOT task_isdeleted "
										 "AND task_description IS NOT NULL "
										 "UNION "
										 "SELECT tcom_task AS task_id, task_parent, lower(tcom_comment) AS search_field "
										 "FROM _taskcomment "
										 "LEFT JOIN _task ON tcom_task = task_id "
										 ") "
										 "SELECT DISTINCT task_id, task_parent, (SELECT task_name FROM _task WHERE task_id = w.task_id) "
										 "FROM w "
										 "WHERE search_field LIKE '%' || :Value || '%' "
										 "ORDER BY task_id");
//-----------------------------------------------------------------------------
ISTaskSearchByTextForm::ISTaskSearchByTextForm(QWidget *parent) : ISInterfaceForm(parent)
{
	setWindowTitle(LANG("Task.SearchByText.Title"));
	resize(ISDefines::Gui::SIZE_640_480);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	EditValue = new ISLineEdit(this);
	EditValue->SetPlaceholderText(LANG("Task.SearchByText.PlaceholderText"));
	EditValue->SetFocus();
	connect(EditValue, &ISLineEdit::DataChanged, this, &ISTaskSearchByTextForm::EditValueChanged);
	LayoutTitle->addWidget(EditValue);

	ButtonSearch = new ISPushButton(BUFFER_ICONS("Search"), LANG("Search"), this);
	ButtonSearch->setEnabled(false);
	connect(ButtonSearch, &ISPushButton::clicked, this, &ISTaskSearchByTextForm::Search);
	LayoutTitle->addWidget(ButtonSearch);

	ListWidget = new ISListWidget(this);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	connect(ListWidget, &ISListWidget::itemDoubleClicked, this, &ISTaskSearchByTextForm::ItemDoubleClicked);
	GetMainLayout()->addWidget(ListWidget);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutBottom);

	LabelResult = new QLabel(this);
	LayoutBottom->addWidget(LabelResult);

	LayoutBottom->addStretch();

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISTaskSearchByTextForm::close);
	LayoutBottom->addWidget(ButtonClose);
}
//-----------------------------------------------------------------------------
ISTaskSearchByTextForm::~ISTaskSearchByTextForm()
{

}
//-----------------------------------------------------------------------------
void ISTaskSearchByTextForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISTaskSearchByTextForm::EnterClicked()
{
	if (EditValue->GetValue().isValid())
	{
		Search();
	}
}
//-----------------------------------------------------------------------------
void ISTaskSearchByTextForm::EditValueChanged()
{
	ButtonSearch->setEnabled(EditValue->GetValue().isValid());
}
//-----------------------------------------------------------------------------
void ISTaskSearchByTextForm::Search()
{
	ISGui::SetWaitGlobalCursor(true);
	ListWidget->Clear();

	ISQuery qSelect(QS_SEARCH);
	qSelect.BindValue(":Value", EditValue->GetValue());
	if (qSelect.Execute())
	{
		int ResultCount = qSelect.GetCountResultRows();
		LabelResult->setText(LANG("Task.SearchByText.ResultLabel").arg(ResultCount));
		while (qSelect.Next())
		{
			int TaskID = qSelect.ReadColumn("task_id").toInt();
			int TaskParentID = qSelect.ReadColumn("task_parent").toInt();
			QString TaskName = qSelect.ReadColumn("task_name").toString();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(TaskParentID ?
				QString("#%1 / #%2: %3").arg(TaskParentID).arg(TaskID).arg(TaskName) :
				QString("#%1: %2").arg(TaskID).arg(TaskName));
			ListWidgetItem->setIcon(BUFFER_ICONS("Task"));
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
			ListWidgetItem->setData(Qt::UserRole, TaskID);
		}

		if (ResultCount > 0)
		{
			ListWidget->setItemSelected(ListWidget->item(0), true);
			ListWidget->setFocus();
		}
	}
	else
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, LANG("Message.Error.TaskSearchByText"), qSelect.GetErrorString());
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISTaskSearchByTextForm::ItemDoubleClicked(QListWidgetItem *ListWidgetItem)
{
	ISGui::ShowTaskViewForm(ListWidgetItem->data(Qt::UserRole).toInt());
}
//-----------------------------------------------------------------------------
