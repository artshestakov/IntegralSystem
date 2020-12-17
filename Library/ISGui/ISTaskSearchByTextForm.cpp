#include "ISTaskSearchByTextForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefinesGui.h"
#include "ISTcpQuery.h"
#include "ISMessageBox.h"
#include "ISGui.h"
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

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
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

	ISTcpQuery qTaskSearchText(API_SEARCH_TASK_TEXT);
	qTaskSearchText.BindValue("Value", EditValue->GetValue());
	if (qTaskSearchText.Execute())
	{
		QVariantList ResultList = qTaskSearchText.TakeAnswer()["Results"].toList();
		LabelResult->setText(LANG("Task.SearchByText.ResultLabel").arg(ResultList.size()));
		for (const QVariant &Variant : ResultList)
		{
			QVariantMap ResultItem = Variant.toMap();
			unsigned int TaskID = ResultItem["ID"].toUInt();
			unsigned int TaskParentID = ResultItem["ParentID"].toUInt();
			QString TaskName = ResultItem["Name"].toString();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(TaskParentID ?
				QString("#%1 / #%2: %3").arg(TaskParentID).arg(TaskID).arg(TaskName) :
				QString("#%1: %2").arg(TaskID).arg(TaskName));
			ListWidgetItem->setIcon(BUFFER_ICONS("Task"));
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
			ListWidgetItem->setData(Qt::UserRole, TaskID);
		}

		if (ResultList.size())
		{
			ListWidget->setItemSelected(ListWidget->item(0), true);
			ListWidget->setFocus();
		}
	}
	else
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, qTaskSearchText.GetErrorString());
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISTaskSearchByTextForm::ItemDoubleClicked(QListWidgetItem *ListWidgetItem)
{
	ISGui::ShowTaskViewForm(ListWidgetItem->data(Qt::UserRole).toInt());
}
//-----------------------------------------------------------------------------
