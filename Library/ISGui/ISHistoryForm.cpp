#include "ISHistoryForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSettings.h"
#include "ISConstants.h"
#include "ISGui.h"
#include "ISMetaData.h"
#include "ISDialogsCommon.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISHistoryForm::ISHistoryForm(QWidget *parent) : ISInterfaceForm(parent)
{
	setWindowTitle(LANG("ISHistoryForm.Title"));
	setWindowIcon(BUFFER_ICONS("History"));
	resize(ISDefines::Gui::SIZE_640_480);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	ISSearchEdit *EditSearch = new ISSearchEdit(this);
	connect(EditSearch, &ISLineEdit::ValueChange, this, &ISHistoryForm::Search);
	GetMainLayout()->addWidget(EditSearch);

	ListWidget = new ISListWidget(this);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	ListWidget->setAlternatingRowColors(true);
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISHistoryForm::Open);
	GetMainLayout()->addWidget(ListWidget);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutBottom);

	LabelRowCount = new QLabel(LANG("ISHistoryForm.RowCount").arg(0), this);
	LayoutBottom->addWidget(LabelRowCount);

	LayoutBottom->addStretch();

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("ISHistoryForm.Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISHistoryForm::close);
	LayoutBottom->addWidget(ButtonClose);
}
//-----------------------------------------------------------------------------
ISHistoryForm::~ISHistoryForm()
{

}
//-----------------------------------------------------------------------------
void ISHistoryForm::AfterShowEvent()
{
	ISInterfaceForm::AfterShowEvent();

	ISTcpQuery qGetHistoryList(API_GET_HISTORY_LIST);
	if (qGetHistoryList.Execute())
	{
		QVariantList VariantList = qGetHistoryList.TakeAnswer()["History"].toList();
		for (const QVariant &Variant : VariantList)
		{
			QVariantMap HistoryMap = Variant.toMap();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(HistoryMap["TableLocalName"].toString() + ": " +
				HistoryMap["ObjectName"].toString() + " (" + HistoryMap["DateTime"].toString() + ')');
			ListWidgetItem->setData(Qt::UserRole, HistoryMap["TableName"]);
			ListWidgetItem->setData(Qt::UserRole * 2, HistoryMap["ObjectID"]);
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 25));
		}
		LabelRowCount->setText(LANG("ISHistoryForm.RowCount").arg(VariantList.size()));
	}
	else
	{
		ISMessageBox::ShowCritical(this, qGetHistoryList.GetErrorString());
	}
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
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, ListWidgetItem->data(Qt::UserRole).toString(), ListWidgetItem->data(Qt::UserRole * 2).toInt()));
}
//-----------------------------------------------------------------------------
