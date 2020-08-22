#include "ISHistoryForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISSettings.h"
#include "ISConstants.h"
#include "ISGui.h"
#include "ISHistory.h"
#include "ISMetaData.h"
#include "ISCore.h"
#include "ISMessageBox.h"
#include "ISCheckEdit.h"
//-----------------------------------------------------------------------------
ISHistoryForm::ISHistoryForm(QWidget *parent) : ISInterfaceForm(parent)
{
	setWindowTitle(LANG("History"));
	setWindowIcon(BUFFER_ICONS("History"));
	resize(ISDefines::Gui::SIZE_640_480);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	ISLineEdit *EditSearch = new ISLineEdit(this);
	EditSearch->SetPlaceholderText(LANG("InputThisSearchQuery"));
	connect(EditSearch, &ISLineEdit::ValueChange, this, &ISHistoryForm::Search);
	GetMainLayout()->addWidget(EditSearch);

	ListWidget = new ISListWidget(this);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	ListWidget->setAlternatingRowColors(true);
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISHistoryForm::Open);
	GetMainLayout()->addWidget(ListWidget);

	std::vector<ISHistoryObject> Stories = ISHistory::Instance().GetHistory();
	for (const ISHistoryObject &HistoryObject : Stories)
	{
		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
		ListWidgetItem->setText(
			ISMetaData::Instance().GetMetaTable(HistoryObject.TableName)->LocalListName + ": " +
			ISCore::GetObjectName(HistoryObject.TableName, HistoryObject.ObjectID) +
			" (" + HistoryObject.DateTime.toString(FORMAT_DATE_TIME_V10) + ')');
		ListWidgetItem->setData(Qt::UserRole, HistoryObject.TableName);
		ListWidgetItem->setData(Qt::UserRole * 2, HistoryObject.ObjectID);
		ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 25));
	}

	ISCheckEdit *CheckEdit = new ISCheckEdit(this);
	CheckEdit->SetText(LANG("EnableHistoryTracking"));
	CheckEdit->SetValue(SETTING_BOOL(CONST_UID_SETTING_OTHER_ENABLE_HISTORY_TRACKING));
	connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISHistoryForm::CheckEditChanged);
	GetMainLayout()->addWidget(CheckEdit, 0, Qt::AlignLeft);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutBottom);

	ISPushButton *ButtonClear = new ISPushButton(BUFFER_ICONS("Clear"), LANG("ClearHistory"), this);
	connect(ButtonClear, &ISPushButton::clicked, this, &ISHistoryForm::Clear);
	LayoutBottom->addWidget(ButtonClear);

	LayoutBottom->addStretch();

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISHistoryForm::close);
	LayoutBottom->addWidget(ButtonClose);
}
//-----------------------------------------------------------------------------
ISHistoryForm::~ISHistoryForm()
{

}
//-----------------------------------------------------------------------------
void ISHistoryForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISHistoryForm::Search(const QVariant &Value)
{
	if (Value.isValid())
	{
		for (int i = 0, c = ListWidget->count(); i < c; ++i)
		{
			if (!ListWidget->item(i)->text().toLower().contains(Value.toString().toLower()))
			{
				ListWidget->setItemHidden(ListWidget->item(i), true);
			}
		}
	}
	else
	{
		ListWidget->SetVisibleItems(true);
	}
}
//-----------------------------------------------------------------------------
void ISHistoryForm::Open(QListWidgetItem *ListWidgetItem)
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, ListWidgetItem->data(Qt::UserRole).toString(), ListWidgetItem->data(Qt::UserRole * 2).toInt()));
}
//-----------------------------------------------------------------------------
void ISHistoryForm::CheckEditChanged(const QVariant &Value)
{
	ISSettings::Instance().SetValue(CONST_UID_SETTING_OTHER_ENABLE_HISTORY_TRACKING, Value);
}
//-----------------------------------------------------------------------------
void ISHistoryForm::Clear()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteHistory")))
	{
		ISGui::SetWaitGlobalCursor(true);
		ISHistory::Instance().Clear();
		ListWidget->Clear();
		ISGui::SetWaitGlobalCursor(false);
	}
}
//-----------------------------------------------------------------------------
